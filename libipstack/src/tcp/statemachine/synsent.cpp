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

#include "tcp/TCP_Socket_Private.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	void TCP_Socket_Private::synsent(TCP_Segment* segment, unsigned len) {
		if (segment != 0) {
			// new tcp segment received:
			// waiting for SYN+ACK

			//extract everything we will need later
			uint32_t seqnum = segment->get_seqnum();
			uint32_t acknum = segment->get_acknum();
			uint16_t window = segment->get_window();
			bool segment_has_ACK = segment->has_ACK();
			bool segment_has_SYN = segment->has_SYN();
			bool segment_has_RST = segment->has_RST();
			freeReceivedSegment(segment); //and delete this packet

			if (segment_has_ACK) {
				if (segment_has_SYN) {
					handleACK(acknum);
					if (seqnum_unacked == seqnum_next) {
						//acknum was correct
						state = ESTABLISHED;
						initSendWindow(window, seqnum, acknum);
						receiveBuffer.setFirstSeqNum(seqnum + 1U);

						clearHistory(); //delete our SYN packet (and thus make sure the following ACK can be allocated)
						//TODO: alternatively, just 'triggerACK()' and let ESTABLISHED transmit it??

						if (sendACK()) {
							waiting = false; //TODO: do that in established? (to clean up the ringbuffer)
						}
						return;
					}
				} else if (segment_has_RST) {
					//"In the SYN-SENT state (a RST received in response
					//to an initial SYN), the RST is acceptable if the ACK field
					//acknowledges the SYN." (rfc793 page 37, Reset Processing)
					if (acknum == seqnum_next) {
						abort(); //RST is valid
					}
					return;
				}
				//[else]
				//acknum incorrect: send a RST with seqNum = (incorrect ackNum)
				//rfc793 page 33 (Figure 9.) ("Recovery from Old Duplicate SYN")
				//-- AND --
				//rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
				//Send a RST an continue connecting (resend SYN after its timeout)
				SendBuffer* b = requestSendBufferTCP();
				if (b != 0) {
					TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
					writeHeader(b);
					segment->set_seqnum(acknum);
					segment->set_RST();
					send(b);
					history.add(b, 0);
				}
			} else if (segment_has_SYN) {
				//rfc793 page 32 (Figure 8.) ("Simultaneous Connection Synchronization")
				//printf("SYN arrived -> Simultanous open !?\n");
				clearHistory(); //delete our SYN packet

				//borrowed form listen.cpp:
				receiveBuffer.setFirstSeqNum(seqnum + 1U);

				//send SYN+ACK:
				SendBuffer* b = requestSendBufferTCP_syn();
				if (b != 0) {
					state = SYNRCVD; //next state only if SYN+ACK can be sent (packet alloc'ed)
					TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
					writeHeaderWithAck(b, receiveBuffer.getAckNum());
					segment->set_seqnum(seqnum_next - 1U); //TODO: correct?
					segment->set_SYN();
					send(b);
					history.add(b, getRTO());
				}
			}
			//else? are there any other corner-cases? --> ignore
		} else {
			// there are no more segments in the input buffer
			updateHistory();
			block(history.getNextTimeout()); //wait for incoming packet OR timeout
		}
	}
}
