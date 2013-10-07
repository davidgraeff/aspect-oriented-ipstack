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


#include "TCP_Socket_Private.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
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

	inline unsigned min(unsigned a, unsigned b, unsigned c) {
		return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
	}

	bool TCP_Socket::sendNextSegment()
	{
		//calculate length of packet to be send as the following minimum:
		return sendSegment(min(getSendWindow(), application_buflen, mss));
	}

	void TCP_Socket::initSendWindow(uint16_t window, uint32_t seqnum, uint32_t acknum) {
		sendWindow = window;
		lwseq = seqnum;
		lwack = acknum;
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

	SendBuffer* TCP_Socket::requestSendBufferTCP_syn(uint_fast16_t payloadsize) {
		return requestSendBufferTCP(payloadsize);
	}

} //namespace ipstack
