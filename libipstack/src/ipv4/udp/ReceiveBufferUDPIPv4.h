// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2012 David Gräff

#pragma once

#include "util/types.h"
#include "util/Mempool.h"
#include "ReceiveBuffer.h"
#include <string.h> //for memcpy

namespace ipstack {

class ReceiveBufferUDPIPv4 : public ReceiveBuffer {
public:
	struct RemoteInfo {
		UInt32 ipv4;
		UInt16 remoteport;
		UInt8 id;
	};
	/**
	 * Create a receive buffer and return it.
	 * Changes the ReceiveBuffer list-head pointer if neccessary.
	 */
	static ReceiveBufferUDPIPv4* createReceiveBufferUDPIPv4(Mempool* mempool, void*  payload, UInt16 payloadsize, UInt16 remoteport, UInt32 ipv4) {
		UInt8 padding = (4 - ((payloadsize + sizeof(RemoteInfo)) % 4)) % 4;
		ReceiveBufferUDPIPv4* r = (ReceiveBufferUDPIPv4*)ReceiveBuffer::createReceiveBuffer(mempool, payload, payloadsize + sizeof(RemoteInfo) + padding);
		if (!r)
			return 0;
		
		r->m_receivedSize = payloadsize;
		char* d = r->m_dataStart;
		d += payloadsize + padding;
		RemoteInfo* remoteinfo = (RemoteInfo*)d;
		remoteinfo->id = '4';
		remoteinfo->remoteport = remoteport;
		remoteinfo->ipv4 = ipv4;
		return r;
	}
	
	static ReceiveBufferUDPIPv4* cast(ReceiveBuffer* b) {
		if (((ReceiveBufferUDPIPv4*)b)->getRemoteInfo()->id != '4')
			return 0;
		return (ReceiveBufferUDPIPv4*)b;
	}
	
	RemoteInfo* getRemoteInfo() {
		UInt8 padding = (4 - ((m_receivedSize + sizeof(RemoteInfo)) % 4)) % 4;
		char* d = m_dataStart;
		d += m_receivedSize + padding;
		return (RemoteInfo*)d;
	}
};

} // namespace ipstack
