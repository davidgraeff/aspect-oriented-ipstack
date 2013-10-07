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
	void TCP_Socket_Private::established(TCP_Segment* segment, unsigned len) {
		if(segment != 0){
			// new tcp segment received:
			
			if(handleRST(segment)){ return; }
			if(handleSYN(segment)){ return; }

			//calculate payload length
			unsigned payload_len = len - (segment->get_header_len()*4);
			uint32_t seqnum = segment->get_seqnum();
			uint32_t acknum = segment->get_acknum();
			
			if(segment->has_ACK()){
			handleACK(acknum);
			}
			
			handleFIN(segment, seqnum, payload_len);
			bool needToFree = handleData(segment, seqnum, payload_len);
			
			updateSendWindow(segment, seqnum, acknum);
			
			if(needToFree == true){
				freeReceivedSegment(segment);
			}
			
			// **********************************************
			if(FIN_complete()){
			//we have received all data so far
			//printf("FIN arrived: ESTABLISHED --> CLOSEWAIT\n");
			state = CLOSEWAIT; //transition: ESTABLISHED --> CLOSEWAIT
			}
			// **********************************************
			
			//respond to "keep-alive" persist window probes
			if(getReceiveWindow() == 0) { triggerACK(); }
			//TODO:
			/*"The proper behaviour of TCP is NOT to respond when it receives a 
			segment with no data and with the next expected sequence number. 
			However, Comer’s code WILL respond to such segment
			(function tcpackit(), p. 303). Zero window probes usually have
			the sequence number one less than expected (Linux) or the expected
			sequence number but only one byte of data (Windows, BSD)."*/
			// [http://simonf.com/tcp/]
			
		}
		else{
			// there are no more segments in the input buffer
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			
			if(receiving == true){
			unsigned recvBytes = receiveBuffer.getRecvBytes();
			if(recvBytes > 0){
				bool pushed = receiveBuffer.isPushed(); //remember here, because copyData removes the pushFlag
				
				unsigned copyBytes = recvBytes > application_buflen ? application_buflen : recvBytes;
				receiveBuffer.copyData(application_buf, copyBytes);
				application_recv_len += copyBytes; //increment recv_len byte count
				application_buf = (void*) (((uint8_t*)application_buf) + copyBytes); //adjust application_buf pointer
				application_buflen -= copyBytes; //adjust data to be still read
				
				processACK();
				
				if( (application_buflen == 0) || (pushed == true) ){
				//return data to the receiving application
				receiving = waiting = false;
				return;
				}
			}
			else{
				processACK();
			}
			//TODO: no buffering requested, urgend data?!
			block();
			}
			else if(waiting == true) {
			//Sending
			processACK();
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