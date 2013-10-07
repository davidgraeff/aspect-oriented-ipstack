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
	void TCP_Socket_Private::listen(TCP_Segment* segment, unsigned len) {
		if (segment != 0) {
			// new tcp segment received:
			if (handleRST(segment)) {
				return;
			}

			//only SYN packet have come here.
			//do not check for SYN flag (once again).
			receiveBuffer.setFirstSeqNum(segment->get_seqnum() + 1U);

			freeReceivedSegment(segment);

			//send SYN+ACK:
			SendBuffer* b = requestSendBufferTCP_syn();
			if (b != 0) {
				state = SYNRCVD; //next state only if SYN+ACK can be sent (packet alloc'ed)
				TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
				writeHeaderWithAck(b, receiveBuffer.getAckNum());
				segment->set_SYN();
				seqnum_next++; // 1 seqnum consumed
				send(b);
				history.add(b, getRTO());
			}
		} else {
			// there are no more segments in the input buffer
			block(); //wait for SYN packet
		}
	}
}