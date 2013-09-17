// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2011 Christoph Borchert


#include "TCP_Socket.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{

TCP_Socket::TCP_Socket(const SocketMemory& memory) : SocketMemory(memory),
	receiveBuffer(this),
	dport(TCP_Segment::UNUSED_PORT),
	sport(TCP_Segment::UNUSED_PORT)
{
	setMaxReceiveWindow(get_Mempool()->getMaxFreeBlockSize());
	resetSocketState();
}

TCP_Socket::~TCP_Socket() {
	unbind();
}

void TCP_Socket::resetSocketState() {
	state = CLOSED;
	receiving = false;
	maxReceiveWindow_MSS = 1;
	maxReceiveWindow_Bytes = TCP_Segment::DEFAULT_MSS;
	FIN_received = false;
	ACK_triggered = false;
	mss = TCP_Segment::DEFAULT_MSS;
	application_buflen = 0;
	waiting = false;
	// Do not reset ports here
}

void TCP_Socket::input(TCP_Segment* segment, unsigned len)
{
	// *THE* state machine switch
	switch (state) {
		case CLOSED: closed(segment, len); break;
		case LISTEN: listen(segment, len); break;
		case SYNSENT: synsent(segment, len); break;
		case SYNRCVD: synrcvd(segment, len); break;
		case ESTABLISHED: established(segment, len); break;
		case FINWAIT1: finwait1(segment, len); break;
		case FINWAIT2: finwait2(segment, len); break;
		case CLOSEWAIT: closewait(segment, len); break;
		case LASTACK: lastack(segment, len); break;
		case CLOSING: closing(segment, len); break;
		case TIMEWAIT: timewait(segment, len); break;
	}
}

uint16_t TCP_Socket::getReceiveWindow()
{
	if (state == SYNSENT || state == CLOSED) {
		return (uint16_t)TCP_Segment::DEFAULT_MSS; //no mss negotiated so far
	} else {
		unsigned currRecvWnd = maxReceiveWindow_Bytes - receiveBuffer.getRecvBytes();
		if (currRecvWnd > 0xFFFFU) {
			//advertised window is only 16 bit wide
			return 0xFFFFU;
		} else {
			return (uint16_t)currRecvWnd;
		}
	}
}

void TCP_Socket::updateSendWindow(TCP_Segment* segment, uint32_t seqnum, uint32_t acknum)
{
	if (TCP_Segment::SEQ_LT(seqnum, lwseq)) {
		return; // this segment arrived out-of-order
		//we aleary have a newer window update
	}
	if ((seqnum == lwseq) &&
			TCP_Segment::SEQ_LT(acknum, lwack)) {
		//this acknowledgment arrived out-of-order
		//we already have received a more recent acknowledgment
		return;
	}
	//else, we have a send window update:

	sendWindow = segment->get_window();
	lwseq = seqnum;
	lwack = acknum;
}

void TCP_Socket::clearHistory()
{
	while (TCP_Record* record = history.get()) {
		get_Mempool()->free(record->getSendBuffer());
		history.remove(record);
	}
}

void TCP_Socket::updateHistory(bool do_retransmit)
{
	//remove TCP segments which are no longer useful from TCP_History
	//and retransmit timed out segements
	Interface* interface = getUsedInterface();
	if (!interface) {
		//FATAL something really got wrong here!
		clearHistory();
		return;
	}
	
	TCP_Record* record = history.get();
	while (record != 0) {
		SendBuffer* buffer = record->getSendBuffer();
		TCP_Segment* segment = (TCP_Segment*)buffer->p.transport_packet;
		uint64_t timeout = record->getTimeout();
		if (((timeout != 0) && (TCP_Segment::SEQ_LT(segment->get_seqnum(), seqnum_unacked))) ||
			((timeout == 0) && interface->hasBeenSent(buffer->getDataStart()))) {
			// 1) Segment is ack'ed (sequence number) OR
			// 2) Segment has no retransmission timeout and has left the network interface
			TCP_Record* next = record->getNext();
			get_Mempool()->free(buffer);
			history.remove(record);
			record = next;
		} else {
			if ((do_retransmit == true) && record->isTimedOut()) {
				//a retransmission timeout was reached
				retransmit(record);
			}
			record = record->getNext();
		}
	}
}

void TCP_Socket::retransmit(TCP_Record* record)
{
	SendBuffer* buffer = record->getSendBuffer();
	buffer->recycle();
	send(buffer);
	record->setTimeout(getRTO());
}

void TCP_Socket::handleACK(uint32_t acknum)
{
	//this segments contains an acknowledgement number
	if (TCP_Segment::SEQ_LEQ(acknum, seqnum_unacked)) {
		//acknum should be at least (seqnum_unacked+1)
		//printf("duplicate ACK arrived\n");
		return; //duplicate ACK
	}
	if (TCP_Segment::SEQ_GT(acknum, seqnum_next)) {
		//acknum must be smaller or equal to seqnum_next
		//printf("ACK outside window arrived!\n");
		if (state == SYNRCVD) {
			//TODO: send a reset (packet with RST flag, ACK?)
		} else {
			//ACK outside window //TODO
		}
		return;
	}
	set_seqnum_unacked(acknum); // seqnums < seqnum_unacked are ack'ed now
}

bool TCP_Socket::handleData(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len)
{
	bool needToFree = true; //caller must free this segment?
	if (payload_len > 0) {
		// this segment contains payload data
		needToFree = (receiveBuffer.insert(segment, seqnum, payload_len) == false);
		triggerACK();
	}
	return needToFree;
}

bool TCP_Socket::sendACK(uint32_t ackNum)
{
	// Create a sendbuffer. Hint: Within TCP_Socket a SendBuffer is created without a tcp header.
	SendBuffer* b = requestSendBufferTCP();
	if (b != 0) {
// 		TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
		writeHeaderWithAck(b, ackNum);
		send(b);
		history.add(b, 0);
		return true;
	}
	return false;
}

bool TCP_Socket::FIN_complete()
{
	//return true if a FIN was received and all data was received completely, too
	if ((FIN_received == true) && (receiveBuffer.getAckNum() == FIN_seqnum)) {
		sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
		return true;
	}
	return false;
}

bool TCP_Socket::handleSYN(TCP_Segment* segment)
{
	//This function is called only at: ESTABLISHED
	if (segment->has_SYN()) {
		// 1) ACK of 'three way handshake' got lost! // && segment->has_ACK())
		// 2) OR: //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
		freeReceivedSegment(segment);
		sendACK(); // 'retransmit'
		return true;
	}
	return false;
}

bool TCP_Socket::handleSYN_final(TCP_Segment* segment)
{
	//call this function if a FIN was received
	//This function is called at: CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT
	if (segment->has_SYN()) {
		//rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
		//TODO: "Internetworking with TCP/IP II page 202": answer a SYN with RST and abort()
		freeReceivedSegment(segment);
		sendACK(FIN_seqnum + 1U);
		return true;
	}
	return false;
}

bool TCP_Socket::handleRST(TCP_Segment* segment)
{
	if (segment->has_RST()) {
		/*In all states except SYN-SENT, all reset (RST) segments are validated
		by checking their SEQ-fields.  A reset is valid if its sequence number
		is in the window. -> TODO */
		freeReceivedSegment(segment);
		abort(); //abort the connection
		return true;
	}
	return false;
}

void TCP_Socket::processACK()
{
	if (ACK_triggered == true) {
		sendACK();
	}
}

void TCP_Socket::writeHeaderWithAck(SendBuffer* sendbuffer, uint32_t ack)
{
	uint8_t headersize = getSpecificTCPHeaderSize();
	TCP_Segment* segment = (TCP_Segment*)sendbuffer->getDataPointer();
	segment->set_dport(dport);
	segment->set_sport(sport);
	segment->set_checksum(0);
	segment->set_seqnum(seqnum_next);
	segment->set_header_len(headersize/4);
	segment->set_flags(0);
	segment->set_window(getReceiveWindow());
	segment->set_urgend_ptr(0);
	segment->set_ACK();
	segment->set_acknum(ack);
	sendbuffer->writtenToDataPointer(headersize);
}

void TCP_Socket::writeHeader(SendBuffer* sendbuffer)
{
	uint8_t headersize = getSpecificTCPHeaderSize();
	TCP_Segment* segment = (TCP_Segment*)sendbuffer->getDataPointer();
	segment->set_dport(dport);
	segment->set_sport(sport);
	segment->set_checksum(0);
	segment->set_seqnum(seqnum_next);
	segment->set_header_len(headersize/4);
	segment->set_flags(0);
	segment->set_window(getReceiveWindow());
	segment->set_urgend_ptr(0);
	//("a standard conforming implementation
	//must set ACK in all packets except for the initial SYN packet")
	segment->set_acknum(0);
	sendbuffer->writtenToDataPointer(headersize);
}

SendBuffer* TCP_Socket::requestSendBufferTCP_syn(uint_fast16_t payloadsize) {
	return requestSendBufferTCP(payloadsize);
}

void TCP_Socket::setMSS(unsigned max_segment_size)
{
	mss = max_segment_size;

	if (sizeof(unsigned) == 2) {
		//8 or 16 bit machine
		uint32_t maxRecvWnd = ((uint32_t)maxReceiveWindow_MSS) * max_segment_size;
		if (maxRecvWnd > 0xFFFFU) {
			//limit to 16 bit (65 KByte)
			maxReceiveWindow_Bytes = 0xFFFFU;
		} else {
			maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
		}
	} else {
		// 32 or 64 bit machine
		uint64_t maxRecvWnd = ((uint64_t)maxReceiveWindow_MSS) * max_segment_size;
		if (maxRecvWnd > 0xFFFFFFFFUL) {
			//limit to 32 bit (4 GByte)
			maxReceiveWindow_Bytes = (unsigned) 0xFFFFFFFFUL;
		} else {
			maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
		}
	}
}

bool TCP_Socket::connect()
{
	if (!hasValidSrcDestAddresses()) {
		return false; //no (valid) dst_ipv4_addr set
	}
	if (isClosed()) {
		//only allow connecting in CLOSED state
		set_sport(TCP_Segment::UNUSED_PORT); //reset source port
		if (bind()) {
			if (state == CLOSED) {
				gen_initial_seqnum();
				sendWindow = 0;

				SendBuffer* b = requestSendBufferTCP_syn();
				if (b != 0) {
					TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
					writeHeader(b);
					segment->set_SYN();
					seqnum_next++; // 1 seqnum consumed
					state = SYNSENT; // next state
					send(b);
					waiting = true;
					history.add(b, getRTO());
				}
			}
			recv_loop();
			return isEstablished();
		}
	}
	return false;
}

void TCP_Socket::abort()
{
	clearHistory(); //free all pending packets
	// clear receive buffer
	while (ReceiveBuffer* t = (ReceiveBuffer*)packetbuffer->get()) {
		ReceiveBuffer::free(t);
	}
	resetSocketState();
}

//full close (no half-close supported)
bool TCP_Socket::close()
{
	if ((state == CLOSEWAIT) || (state == ESTABLISHED)) {
		//send FIN:
		SendBuffer* b = requestSendBufferTCP();
		if (b != 0) {
			TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
			writeHeaderWithAck(b, receiveBuffer.getAckNum());
			segment->set_FIN();
			seqnum_next++; // 1 seqnum consumed
			send(b);
			waiting = true;
			history.add(b, getRTO());
			if (state == CLOSEWAIT) {
				state = LASTACK; //passive close
			} else { //if(state == ESTABLISHED)
				state = FINWAIT1; //active close
			}
		}
	}
	//else{
	//  printf("ERROR: close() must be in ESTABLISHED or CLOSEWAIT\n");
	//}
	recv_loop();
	unbind();
	return isClosed();
}

bool TCP_Socket::block(uint32_t timeout)
{
	return false;
}

void TCP_Socket::block() {} //wait for incoming packets only


void TCP_Socket::processSendData()
{
	if (application_buflen == 0)
		return;

	//there is data to be sent!

	if (sendWindow == 0) { // (!) do NOT use getSendWindow(), because it may be affected by congestion avoidance aspects!
		//sendWindow is closed. we cannot send any data
		//probe for 'window update'
		if (history.getNextTimeout() == 0) {
			//only probe for zero window if there is no other packet
			//in history which triggers an timeout event soon
			// -> send BSD style zero window probe
			//    (expected sequence number but only one byte of data)
			sendSegment(1);
		}
		return;
	}

	else {
		//sendWindow is opened
		while ((application_buflen > 0) && (getSendWindow() > 0)) {
			//TODO: propagate sendWindow below?!
			//we have data be be sent AND receiver can handle new packets
			if (sendNextSegment() == false) {
				//stop further sending. an error (e.g. no buffer, silly window, ...) occured!
				return;
			}
		}
	}
}
bool TCP_Socket::sendSegment(uint_fast16_t len) {
	// Create a sendbuffer. Hint: Within TCP_Socket a SendBuffer is created without a tcp header.
	SendBuffer* b = requestSendBufferTCP(len);
	if (b != 0) {
		TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
		writeHeaderWithAck(b, receiveBuffer.getAckNum());

		b->write(application_buf, len);
		application_buflen -= len;
		application_buf = (void*)(((uint8_t*)application_buf) + len);
		
		//set PUSH flag on last segment
		if (application_buflen == 0) {
			segment->set_PSH();
		}
		
		// TCP state update
		seqnum_next += len;
		// Because this method can also be used if the send window is closed for a "zero window probe"
		// the sendWindow variable has to be checked before lowering it.
		if (sendWindow) 
			lowerSendWindow(len); //subtract len bytes from sendWindow
		
		send(b);
		history.add(b, getRTO());
		return true;
	}
	return false; //can't get buffer
}

bool TCP_Socket::sendNextSegment()
{
	//calculate length of packet to be send as the following minimum:
	return sendSegment(min(getSendWindow(), application_buflen, mss));
}
void TCP_Socket::set_dport(uint16_t d)
{
	dport = d;
}
uint16_t TCP_Socket::get_dport()
{
	return dport;
}
void TCP_Socket::set_sport(uint16_t s)
{
	sport = s;
}
uint16_t TCP_Socket::get_sport()
{
	return sport;
}

void TCP_Socket::setMaxReceiveWindow(unsigned max) {
	maxReceiveWindow_MSS = max;
}

bool TCP_Socket::listen()
{
	if (state == CLOSED) {
		gen_initial_seqnum();
		sendWindow = 0;
		state = LISTEN; // next state
		waiting = true;
		return true;
	}
	return false;
}
void TCP_Socket::gen_initial_seqnum()
{
	//RFC 793 (TCP Illustrated Vol.1 page 232)
	uint64_t usec = Clock::ticks_to_ms(Clock::now()) * 1000UL; // microseconds
	seqnum_next = usec / 4; // 'increment' every 4 usec ;-)
	seqnum_unacked = seqnum_next;
}
void TCP_Socket::recv_loop()
{
	while (waiting_for_input()) {
		ReceiveBuffer* receiveB = (ReceiveBuffer*)packetbuffer->get();
		if (receiveB != 0) {
			input((TCP_Segment*) receiveB->getData(), receiveB->getSize());
		} else {
			input(0, 0);
		}
	}
}
int TCP_Socket::poll(unsigned int msec)
{
	if (isEstablished() || isCloseWait()) {
		ReceiveBuffer* receiveB = (ReceiveBuffer*)packetbuffer->get();
		if (receiveB == 0) {
			//no gratuitous packets received, yet
			bool timeout_reached = (msec==0) ? false : block((uint32_t)msec); //wait for max. msec
			if (timeout_reached == false) {
				receiveB = (ReceiveBuffer*)packetbuffer->get(); //check for new packet, since timeout was not reached
			}
		}
		if (receiveB != 0) {
			do {
				//process all gratuitous packets that have arrived
				input((TCP_Segment*) receiveB->getData(), receiveB->getSize());
				receiveB = (ReceiveBuffer*)packetbuffer->get();
			} while (receiveB != 0);
			updateHistory(); //cleanup packets which are not used anymore
			processACK(); //send ACK if necessary
		}

		//return amount of received data bytes (payload) while polling
		//zero out MSB, because return value is an int!
		const unsigned msb = 1 << ((8 * sizeof(unsigned)) - 1);
		return (int)(getRecvBytes() & ~msb);
	}
	return -1;
}
int TCP_Socket::receive(void* buffer, unsigned int buffer_len)
{
	if (isEstablished() || isCloseWait()) {
		//zero out MSB, because return value is an int!
		const unsigned msb = 1 << ((8 * sizeof(unsigned)) - 1);
		buffer_len &= ~msb;
		application_buf = buffer;
		application_buflen = buffer_len;
		application_recv_len = 0;
		receiving = true;
		waiting = true;
		recv_loop();
		unsigned recv_len = get_application_recv_len();
		if ((recv_len == 0) && isCloseWait()) {
			return -2;
		}
		return (int)recv_len;
	}
	return -1;
}
bool TCP_Socket::write(const void* data, unsigned int datasize)
{
	if (isEstablished() || isCloseWait()) {
		application_buf = (void*) data;
		application_buflen = datasize;
		waiting = true;
		recv_loop();
		return (isEstablished() || isCloseWait()); //we are still is a state that allows sending
	}
	return false;
}
bool TCP_Socket::bind()
{
	return Demux::Inst().bind(this);
}
void TCP_Socket::unbind()
{
	Demux::Inst().unbind(this);
}
SendBuffer* TCP_Socket::requestSendBufferTCP(uint_fast16_t payloadsize)
{
	if (history.isFull()) {
		updateHistory(false); //update history ... (false := do not trigger any retransmissions)
		if (history.isFull()) { // ... and try again
			return 0;
		}
	}

	return requestSendBuffer(getSpecificTCPHeaderSize() + payloadsize);
}
bool TCP_Socket::addToReceiveQueue(TCP_Segment* segment, unsigned int segment_len)
{
	// We have to use the ReceiveBuffer wrapper here instead of copying the
	// segment and putting the new pointer into the packetbuffer because
	// we need the length.
	ReceiveBuffer* receiveB = ReceiveBuffer::createReceiveBuffer(get_Mempool(), segment, segment_len);
	if (receiveB != 0) {
		packetbuffer->put(receiveB);
		return true;
	}
	return false;
}
void TCP_Socket::freeReceivedSegment(TCP_Segment* segment)
{
	ReceiveBuffer* receiveB = (ReceiveBuffer*)((char*)segment - sizeof(ReceiveBuffer));
	ReceiveBuffer::free(receiveB);
}


} //namespace ipstack
