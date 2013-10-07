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
	void TCP_Socket_Private::closing(TCP_Segment* segment, unsigned len) {
		if(segment != 0){
			// new tcp segment received:
			if(handleRST(segment)){ return; }
			if(handleSYN_final(segment)){ return; }
			
			if(segment->has_ACK()){
			handleACK(segment->get_acknum()); //Just wait for an ACK (for our FIN)
			}
			
			if(segment->has_FIN()){
			//Our ACK from state FINWAIT1 got lost. retransmit!
			//updateHistory(false); // free unused segments stored in history //XXX: now done in alloc()
			sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
			}
			
			freeReceivedSegment(segment);
		}
		else{
			// there are no more segments in the input buffer
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			if(history.isEmpty()){
			// Our FIN got ack'ed
			//printf("ACK arrived: CLOSING --> TIMEWAIT\n");
			state = TIMEWAIT;
			}
			else{
			//resend FIN (from close()) after timeout
			block(history.getNextTimeout());
			}
		}
	}
}