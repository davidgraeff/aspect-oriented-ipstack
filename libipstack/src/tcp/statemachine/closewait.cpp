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
	void TCP_Socket_Private::closewait(ReceiveBuffer* receiveB) {
		if(receiveB){
			TCP_Segment* segment = (TCP_Segment*) receiveB->getData();
			
			// new tcp segment received:
			if(segment->has_RST()) {handleRST(receiveB); return; } 
			if(segment->has_SYN()) {handleSYN_final(receiveB); return; } 
			
			uint32_t seqnum = segment->get_seqnum();
			uint32_t acknum = segment->get_acknum();
			
			if(segment->has_ACK()){
				handleACK(acknum);
			}
			
			if(segment->has_FIN()){
				//updateHistory(false); // free unused segments stored in history //XXX: now done in alloc()
				//Our ACK from state ESTABLISHED got lost. retransmit!
				sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
			}
			
			updateSendWindow(segment, seqnum, acknum);
			freeReceivebuffer(receiveB);
		}
		else{
			// there are no more segments in the input buffer
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			
			if(receiving == true){
				//There will be no more incoming data segments.
				// => flush receiveBuffer if possible
				unsigned recvBytes = receiveBuffer.getRecvBytes();
				if(application_buflen > recvBytes){
					application_buflen = recvBytes; //only copy recvBytes
				}
				application_recv_len += application_buflen; //increment recv_len byte count
				receiveBuffer.copyData(application_buf, application_buflen);
				receiving = waiting = false; //never block on reading in this state
				return;
				//TODO: urgend data?
			}
			else if(waiting == true) {
				//Sending
				processSendData(); //try to send new segments if possible
				if( (application_buflen == 0) && history.isEmpty()){
					waiting = false;
					return;
				}
				else{
					block(history.getNextTimeout());
				}
			}
		}
	}
}
