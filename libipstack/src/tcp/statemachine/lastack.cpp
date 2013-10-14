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
	void TCP_Socket_Private::lastack(ReceiveBuffer* receiveB) {
		if(receiveB){
			TCP_Segment* segment = (TCP_Segment*) receiveB->getData();
			
			//Just wait for an ACK (for our FIN)
			if(segment->has_RST()) {handleRST(receiveB); return; } 
			if(segment->has_SYN()) {handleSYN_final(receiveB); return; } 
			if(segment->has_ACK()){ handleACK(segment->get_acknum()); }
			
			freeReceivebuffer(receiveB);
		} else {
			// there are no more segments in the input buffer
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			if(history.isEmpty()){
				// Our FIN got ack'ed --> connection fully terminated now
				abort(); //kill connection
			} else {
				//resend FIN(+ACK) after a timeout
				block(history.getNextTimeout());
			}
		}
	}
}
