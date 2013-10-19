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
	void TCP_Socket_Private::synrcvd(ReceiveBuffer* receiveB) {
		if(receiveB){
			TCP_Segment* segment = (TCP_Segment*) receiveB->getData();
			
			if(segment->has_RST()) {handleRST(receiveB); return; } 

			if (segment->has_ACK()) {
				uint32_t acknum = segment->get_acknum();
				handleACK(acknum);
				if (acknum == seqnum_next) {
					//This segment ack'ed our SYN
					//printf("ACK arrived: SYNRCVD --> ESTABLISHED\n");
					state = ESTABLISHED;
					initSendWindow(segment->get_window(), segment->get_seqnum(), acknum);
					clearHistory(); //no more packets to retransmit
					waiting = false; //TODO: do that in established? (to clean up the ringbuffer)
				}
			} else if (segment->has_SYN()) {
				//Our SYN+ACK (from state LISTEN or SYNSENT) got lost
				TCP_Record* record = history.get();
				if (record != 0) {
					retransmit(record);
				} else {
					//TODO: abort connection here?
				}
			}
			freeReceivebuffer(receiveB);
		} else {
			// there are no more segments in the input buffer
			//TODO: goto CLOSED after timeout (no R1, R2?)
			//for a timeout, we need another member variable :-(
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			block(history.getNextTimeout());
		}
	}
}
