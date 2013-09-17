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
// Copyright (C) 2013 David Gräff

#include "udp/UDP_Packet.h"
#include "udp/UDP_Socket.h"
#include "demux/Demux.h"
#include <string.h> //for memcpy
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

/**
 * Purspose: add bind/unbind to the socket for demux delegated methods.
 * Delegate convenience API methods for receiving. The real implementations
 * are in the IP(v4/v6) receiving slices.
 */

namespace ipstack {

	bool UDP_Socket::bind() {
		return Demux::Inst().bind ( &socket );
	}
	
	void UDP_Socket::unbind() {
		Demux::Inst().unbind ( &socket );
	}

    bool UDP_Socket::addToReceiveQueue ( ReceiveBuffer& receivebuffer) {
		ReceiveBuffer* socket_receive_buffer;
		if (is_packetbuffer_full() || !(socket_receive_buffer=receivebuffer.clone(this))) {
			return false;
		}
		packetbuffer->put(socket_receive_buffer);
		return true;
	}
	
	void UDP_Socket::block() {}
	
	SmartReceiveBufferPtr UDP_Socket::receive(){
		return SmartReceiveBufferPtr((ReceiveBuffer*)packetbuffer->get(), this);
	}

	SmartReceiveBufferPtr UDP_Socket::receive(uint64_t waitForPacketTimeoutMS) {
		ReceiveBuffer* recv = (ReceiveBuffer*)packetbuffer->get();
		if (!recv) {
			if (!waitForPacketTimeoutMS )
				return 0;
			uint64_t timeout = Clock::now() + Clock::ms_to_ticks(waitForPacketTimeoutMS);
			while(timeout < Clock::now()){
				recv = (ReceiveBuffer*)packetbuffer->get();
				if (recv)
					break;
			}
		}
		return SmartReceiveBufferPtr(recv, this);
	}
	
	SmartReceiveBufferPtr UDP_Socket::receiveBlock(){
		ReceiveBuffer* recv = (ReceiveBuffer*)packetbuffer->get();
		while(recv == 0){
			block();
			recv = (ReceiveBuffer*)packetbuffer->get();
		}
		return SmartReceiveBufferPtr(recv, this);
	}
}