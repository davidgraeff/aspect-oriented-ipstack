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

#include "../cfAttribs.h"
#include "../api/Setup.h"
#include "../api/UDP_Socket.h"
#include "ipstack/SendBuffer.h"

namespace ipstack {

class UDPDebugPort {
	public:
		// You need an activated IPv4 interface!
		static void debugstring(const char* msg, UInt8 len) {
			static IP::UDP_Socket<280,1,0,0> socket;
			socket.ipv4.set_dst_addr(cfIPSTACK_UDP_DEBUGPORT_DESTIP);
			socket.set_dport(cfIPSTACK_UDP_DEBUGPORT_DESTPORT);
			
			SendBuffer outBuffer = socket.requestSendBuffer(len);
			
			// didn't get buffer, maybe "len" is to great or no ip interface is up -> abort
			if (!outBuffer.data)
				return;
			
			outBuffer.write(msg, len);
			socket.send(outBuffer);
		}
};

}
