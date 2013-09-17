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

#include "memory_management/MemoryInterface.h"
#include "router/Interface.h"
#include "util/ipstack_inttypes.h"
#include "util/protocol_layers.h"
#include "SendBufferPrivate.h"

namespace ipstack
{
	class ReceiveBuffer;
	/**
	 * A sendbuffer is used for all sending activity within the tcp/ip stack. It is manipulated and filled
	 * by different aspects of the network stack layers. The size for the buffer has to be known beforehand
	 * to allocate the memory.
	 * You have to request a send-buffer to send data via send(...) in sockets.
	 * 
	 * Write your payload either to the data address like in this example:
	 * Socket s;
	 * SendBuffer* b = s.requestSendBuffer(size);
	 * if (!b) ABORT();
	 * memcpy(b->getDataPointer(), "test", 4);
	 *
	 * You may omit memcpy for integers for example with this code:
	 * ((uint32_t*)b->getDataPointer()) = 47248459;
	 * b->writtenToDataPointer(size_of(uint32_t)); // tell the sendbuffer how many bytes you have written
	 * s.send(b);
	 * 
	 * Another way of writting to a sendbuffer is using the write method:
	 * write(const char* data, uint16_t length);
	 * 
	 * Always check if you got a null pointer as a result. This indicates your
	 * requested size is to big or there is no memory chunk of this size
	 * available at the moment. Wait and try it later.
	 * 
	 * A SendBuffer object is invalid after calling send with it. You have to
	 * request a new one for more data to send. YOu may also use recycle() under
	 * some conditions.
	 * 
	 * Always free a SendBuffer after sending it (if you do not want to recycle it).
	 * Call socket.free(sendBuffer);
	 * 
	 */
class SendBuffer : public SendBufferPrivate
{
	public:
		/**
		  * Create a new SendBuffer object and allocate memory for it. If not enough memory is available 0
		  * is returned. You should provide the destination interface. It is recommend to use the convenience
		  * methods of yout socket instead of a raw SendBuffer directly.
		  */
		static SendBuffer* createInstance(MemoryInterface* mem, uint_fast16_t requestedSize, Interface* interface);

		/**
		 * Put your data here.
		 * Example:
		 * SendBuffer* buffer = socket.requestSendBuffer(4);
		 * memcpy(buffer->getDataPointer(), "test", 4);
		 * OR
		 * *((int32*)buffer->getDataPointer()) = 1472984;
		 * Always call writtenToDataPointer after writting to this memory location!
		 */
		void* getDataPointer();

		/**
		 * Use this method to increment the data pointer after writing to it directly
		 */
		void writtenToDataPointer(uint_fast16_t length);

		/**
		 * A convenient method for writing to this SendBuffer.
		 * This is less efficient if you incrementally compute data and aggregate those instead
		 * of directly write to the data pointer in incremental steps.
		 */
		void write(const void* newdata, uint_fast16_t length);

		/**
		  * A sendbuffer may be recycled to send the identical content again or may be
		  * be modified before sending again. The size of the sendbuffer is fixed to the
		  * initial requested size though.
		  * This method blocks until interface->hasBeendSend() returns true for this buffer.
		  */
		void recycle();

		/**
		 * Return the size in bytes that is available for your data.
		 * Cache this value!
		 * */
		uint_fast16_t getRemainingSize();
		uint_fast16_t getSize();
		
		/**
		 * Contains pointer to protocols. Example:
		 * SendBuffer b;
		 * // b is filled somehow
		 * b.p.ipv4_packet->get_dest_ip();
		 */
		class protocol_layers {} p;
	private:
		explicit SendBuffer(); // private constructor
		SendBuffer(const SendBuffer& s); // private copy constructor
		inline void* getDataStart() {return ((char*)this) + sizeof(SendBuffer);}
};

} // namespace ipstack
