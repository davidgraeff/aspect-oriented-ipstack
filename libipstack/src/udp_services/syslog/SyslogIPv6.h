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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "IPStack_Config.h"
#include "router/sendbuffer/SendBuffer.h"
#include "udp/UDP_Socket.h"
#include "memory_management/Mempool_Instance_UDP.h"

namespace ipstack {

class UDPDebugPort {
	public:
		// You need an activated IPv4 interface!
		static void debugstring(const char* msg, uint8_t len) {
			static Mempool_Instance_UDP memInstance = Mempool_Instance_UDP<280,1,0,0>();
			static UDP_Socket socket(memInstance);
			socket.ipv6.set_dst_addr(cfIPSTACK_UDP_DEBUGPORT_DESTIPv6);
			socket.set_dport(cfIPSTACK_UDP_DEBUGPORT_DESTPORT);
			
			SendBuffer* outBuffer = socket.requestSendBuffer(len);
			
			// didn't get buffer, maybe "len" is to great or no ip interface is up -> abort
			if (!outBuffer)
				return;
			
			outBuffer->write(msg, len);
			socket.send(outBuffer);
		}
};

}
