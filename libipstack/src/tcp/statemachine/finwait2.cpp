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
	void TCP_Socket_Private::finwait2(TCP_Segment* segment, unsigned len) {
		if(segment != 0){
			// new tcp segment received:
			
			if(handleRST(segment)){ return; }
			if(handleSYN(segment)){ return; }
			
			//calculate payload length
			unsigned payload_len = len - (segment->get_header_len()*4);
			uint32_t seqnum = segment->get_seqnum();
			
			handleFIN(segment, seqnum, payload_len);
			bool needToFree = handleData(segment, seqnum, payload_len);
			
			if(needToFree == true){
				freeReceivedSegment(segment);
			}
			
			// **********************************************
			if(FIN_complete()){
			//we have received all data so far
			//printf("FIN arrived: FINWAIT2 --> TIMEWAIT\n");
			state = TIMEWAIT;
			}
			// **********************************************    
		}
		else{
			// there are no more segments in the input buffer
			updateHistory();
			processACK(); //send ACK if requested
			block();
			//TODO: optional FIN_WAIT2_Timeout!! -> Aspect?
			//No support for half-close, because an incoming FIN must be handled
		}
	}
}
