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
// Copyright (C) 2012 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"
#include "util/MemoryInterface.h"
#include "receive/ReceiveBuffer.h"
#include "ipv6/IPv6AddressUtilities.h"
#include <string.h> //for memcpy

namespace ipstack {

class ReceiveBufferUDPIPv6 : public ReceiveBuffer {
public:
	struct RemoteInfo {
		uint8_t id;
		uint16_t remoteport;
		ipv6addr ipv6;
	};
	/**
	 * Create a receive buffer and return it.
	 * Changes the ReceiveBuffer list-head pointer if neccessary.
	 */
	static ReceiveBufferUDPIPv6* createReceiveBufferUDPIPv6(MemoryInterface* mem, void*  payload, uint16_t payloadsize, uint16_t remoteport, ipv6addr ipv6) {
		ReceiveBufferUDPIPv6* r = (ReceiveBufferUDPIPv6*)ReceiveBuffer::createReceiveBuffer(mem, payload, payloadsize + sizeof(RemoteInfo));
		if (!r)
			return 0;
		
		r->m_receivedSize = payloadsize;
		char* d = r->m_dataStart;
		d+= payloadsize;
		RemoteInfo* remoteinfo = (RemoteInfo*)d;
		remoteinfo->id = '6';
		remoteinfo->remoteport = remoteport;
		copy_ipv6_addr(ipv6, remoteinfo->ipv6);
		return r;
	}
	
	static ReceiveBufferUDPIPv6* cast(ReceiveBuffer* b) {
		if (((ReceiveBufferUDPIPv6*)b)->getRemoteInfo()->id != '6')
			return 0;
		return (ReceiveBufferUDPIPv6*)b;
	}
	
	RemoteInfo* getRemoteInfo() {
		char* d = m_dataStart;
		d+= m_receivedSize;
		return (RemoteInfo*)d;
	}
};

} // namespace ipstack
