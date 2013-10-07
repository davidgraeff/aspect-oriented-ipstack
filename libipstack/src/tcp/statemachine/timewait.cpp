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
	void TCP_Socket_Private::timewait(TCP_Segment* segment, unsigned len) {
		if (segment != 0) {
			// new tcp segment received:
			if (handleRST(segment)) {
				return;
			}
			if (handleSYN_final(segment)) {
				return;
			}

			if (segment->has_FIN()) {
				//Our ACK from laste state (FINWAIT1, FINWAIT2) got lost -> retransmit
				sendACK(FIN_seqnum + 1U);
			}
			freeReceivedSegment(segment);
		} else {
			// there are no more segments in the input buffer
			updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
			if (block(2 * getRTO()) == true) {
				//----
				//NOTE: This is NOT conform to rfc1122,
				//but since there is no concurrency here,
				//waiting 2 min (=2MSL) in userspace is quite a long time ...
				//Just waiting for 2*RTO seems to be sufficient.
				//----
				// NOTE2: For a "webserver" use-case the timeout is still to high.
				// Should be configurable maybe?
				
				abort(); //kill connection
			}
			//No support for half-close, because an incoming FIN must be handled
		}
	}
}
