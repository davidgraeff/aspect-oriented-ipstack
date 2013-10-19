
		
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

#include "SocketMemory.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	SocketMemory::SocketMemory(MemoryInterface* m, RingbufferInterface* pb) : mempool(m), packetbuffer(pb) {}

	/**
	 * Free a Sendbuffer or Receivebuffer by using these methods. 
	 */
	void SocketMemory::freeReceivebuffer(void* b) {
		get_Mempool()->free(b);
	}
	void SocketMemory::freeSendbuffer(void* b) {
		get_Mempool()->free(b);
	}
	
	/**
	 * Copies a receivebuffer memory content to the memory of this socket.
	 * This may be intercepted by aspects for directly processing the packet
	 * without copying the content before and by the IPv4 fragmentation
	 * reassamle aspect.
	 */
    bool addToReceiveQueue(ReceiveBuffer& receivebuffer) {
		ReceiveBuffer* socket_receive_buffer;
		if (is_packetbuffer_full() || !(socket_receive_buffer=receivebuffer.clone(this))) {
			return false;
		}
		packetbuffer->put(socket_receive_buffer);
		return true;
	}
	
	void SocketMemory::block() {}
	
	SmartReceiveBufferPtr SocketMemory::receive(){
		return SmartReceiveBufferPtr(receiveRawPointer(), this);
	}

	SmartReceiveBufferPtr SocketMemory::receive(uint64_t waitForPacketTimeoutMS) {
		ReceiveBuffer* recv = receiveRawPointer();
		if (!recv) {
			if (!waitForPacketTimeoutMS )
				return 0;
			uint64_t timeout = Clock::now() + Clock::ms_to_ticks(waitForPacketTimeoutMS);
			while(timeout < Clock::now()){
				recv = receiveRawPointer();
				if (recv)
					break;
			}
		}
		return SmartReceiveBufferPtr(recv, this);
	}
	
	SmartReceiveBufferPtr SocketMemory::receiveBlock(){
		ReceiveBuffer* recv = receiveRawPointer();
		while(recv == 0){
			block();
			recv = receiveRawPointer();
		}
		return SmartReceiveBufferPtr(recv, this);
	}
	
	ReceiveBuffer* SocketMemory::receiveRawPointer() {
		return (ReceiveBuffer*)packetbuffer->get();;
	}
}