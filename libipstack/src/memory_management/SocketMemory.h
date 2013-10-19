
		
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

#pragma once

#include "memory_management/MemoryInterface.h"

namespace ipstack
{
	/**
	* Interface for beeing used in a socket class
	* to include memory management.
	*/
	class SocketMemory
	{
	public:
		SocketMemory(MemoryInterface* m, RingbufferInterface* pb=0);
		
		// Ringbuffer getter/setter
		inline RingbufferInterface* get_packetbuffer() { return packetbuffer; }
		inline bool is_packetbuffer_full() {return packetbuffer->isFull();}
		inline void set_packetbuffer(RingbufferInterface* pb) { packetbuffer = pb; }
		// Memory pool getter/setter
		inline void set_Mempool(MemoryInterface* m) { mempool = m; }
		inline MemoryInterface* get_Mempool() { return mempool; }
		
		/**
		* Free a Sendbuffer or Receivebuffer by using these methods. 
		*/
		void freeReceivebuffer(void* b);
		void freeSendbuffer(void* b);
		
		/**
		* Copies a receivebuffer memory content to the memory of this socket.
		* This may be intercepted by aspects for directly processing the packet
		* without copying the content before and by the IPv4 fragmentation
		* reassamle aspect.
		*/
		bool addToReceiveQueue(ReceiveBuffer& receivebuffer);
		
		/**
		* Return a smart pointer to a ReceiveBuffer. You can access the payload by using
		* the get_payload_data() method and get the size by get_payload_size().
		* 
		* @return This method will return an invalid smart pointer (=0)
		* if no received data is available and will never block.
		* 
		* This method is not implemented, if you have disabled receive support.
		* 
		* Example usage:
		* SmartReceiveBufferPtr b = socket->receive();
		* printf(b->getData());
		*/
		inline SmartReceiveBufferPtr receive();
		
		/**
		* Convenience method: Block until a packet is available or a timeout is reached.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled receive support.
		*/
		SmartReceiveBufferPtr receive(uint64_t waitForPacketTimeoutMS);
		
		/**
		* Convenience method: Block until a packet is available.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled receive support.
		*/
		SmartReceiveBufferPtr receiveBlock();
		
		ReceiveBuffer* receiveRawPointer();
		void block();

	protected:
		MemoryInterface* mempool;
		RingbufferInterface* packetbuffer;
	};
}