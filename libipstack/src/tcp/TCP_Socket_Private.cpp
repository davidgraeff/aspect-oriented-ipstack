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


#include "TCP_Socket_Private_Private.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	void TCP_Socket_Private::resetSocketState() {
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

	void TCP_Socket_Private::abort()
	{
		// Free all pending packets
		clearHistory();
		// Clear receive buffer
		while (ReceiveBuffer* t = receiveRawPointer()) {
			ReceiveBuffer::free(t);
		}
		// Unbind and reset socket state
		unbind();
		resetSocketState();
	}

	void TCP_Socket_Private::input(ReceiveBuffer* receiveB)
	{
		// *THE* state machine switch
		switch (state) {
			case CLOSED: closed(receiveB); break;
			case LISTEN: listen(receiveB); break;
			case SYNSENT: synsent(receiveB); break;
			case SYNRCVD: synrcvd(receiveB); break;
			case ESTABLISHED: established(receiveB); break;
			case FINWAIT1: finwait1(receiveB); break;
			case FINWAIT2: finwait2(receiveB); break;
			case CLOSEWAIT: closewait(receiveB); break;
			case LASTACK: lastack(receiveB); break;
			case CLOSING: closing(receiveB); break;
			case TIMEWAIT: timewait(receiveB); break;
		}
	}

	void TCP_Socket_Private::handleACK(uint32_t acknum)
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
			if (isSynRcvd()) {
				//TODO: send a reset (packet with RST flag, ACK?)
			} else {
				//ACK outside window //TODO
			}
			return;
		}
		set_seqnum_unacked(acknum); // seqnums < seqnum_unacked are ack'ed now
	}

	bool TCP_Socket_Private::sendACK(uint32_t ackNum)
	{
		// Create a sendbuffer. Hint: Within TCP_Socket_Private a SendBuffer is created without a tcp header.
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

	bool TCP_Socket_Private::isFinFlag_and_complete()
	{
		//return true if a FIN was received and all data was received completely, too
		if ((FIN_received == true) && (receiveBuffer.getAckNum() == FIN_seqnum)) {
			sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
			return true;
		}
		return false;
	}

	void TCP_Socket_Private::handleFIN(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len) {
		if (segment->has_FIN() && (FIN_received == false)) {
			FIN_seqnum = seqnum + payload_len;
			FIN_received = true;
		}
	}
		
	void TCP_Socket_Private::handleSYN(ReceiveBuffer* receiveB)
	{
		//This function is called only at: ESTABLISHED
		// 1) ACK of 'three way handshake' got lost! // && segment->has_ACK())
		// 2) OR: //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
		freeReceivebuffer(receiveB);
		sendACK(); // 'retransmit'
	}

	void TCP_Socket_Private::handleSYN_final(ReceiveBuffer* receiveB)
	{
		//call this function if a FIN was received
		//This function is called at: CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT
		//rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
		//TODO: "Internetworking with TCP/IP II page 202": answer a SYN with RST and abort()
		freeReceivebuffer(receiveB);
		sendACK(FIN_seqnum + 1U);
	}

	void TCP_Socket_Private::handleRST(ReceiveBuffer* receiveB)
	{
		/*In all states except SYN-SENT, all reset (RST) segments are validated
		by checking their SEQ-fields.  A reset is valid if its sequence number
		is in the window. -> TODO */
		freeReceivebuffer(receiveB);
		abort(); //abort the connection
	}

	void TCP_Socket_Private::processACK()
	{
		if (ACK_triggered == true) {
			sendACK();
		}
	}

	bool TCP_Socket_Private::block(uint32_t timeout)
	{
		return false;
	}

	void TCP_Socket_Private::gen_initial_seqnum()
	{
		//RFC 793 (TCP Illustrated Vol.1 page 232)
		uint64_t usec = Clock::ticks_to_ms(Clock::now()) * 1000UL; // microseconds
		seqnum_next = usec / 4; // 'increment' every 4 usec ;-)
		seqnum_unacked = seqnum_next;
	}
} //namespace ipstack
