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

namespace ipstack {
class ReceiveBuffer;
class SendBuffer : public SendBufferPrivate
{
	public:
		/**
		  * Allocate memory and if that is successful, create a thin wrapper ("sendbuffer")
		  * around it.
		  * @param mem The socket memory, the buffer will be allocated on.
		  * @param interface You should provide the destination interface. Necessary for
		  * interface->hasBeenSend(memory).
		  * @param requestedSize The buffer size.
		  * @return Return a sendbuffer wrapper object around a free memory block.
		  * If not enough memory is available 0 is returned.
		  */
		static SendBuffer* requestRawBuffer(SocketMemory& mem, Interface* interface, uint_fast16_t requestedSize);

		/**
		  * Send the data of a SendBuffer. The packet is invalid after sending it
		  * and may be freed by calling socket->freeSendbuffer(sendbuffer).
		  */
		bool send();
		
		/**
		 * Put your data here.
		 */
		void* getDataPointer();

		/**
		 * Use this method to increment the data pointer after writing to it directly.
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
		 */
		uint_fast16_t getRemainingSize();
		uint_fast16_t getSize();
		
		/**
		  * For debugging purposes you may associate a string with this sendbuffer. You have to enable
		  * the SendBuffer Debugging feature for an actual output. If not enabled all mark methods in the
		  * code should be optimised out by the compiler (only use const char strings!).
		  * @param str The string to be used for debug messages.
		  */
		inline void mark(const char* str) {}
		
		/**
		 * Contains pointer to protocols. Example:
		 * SendBuffer b;
		 * // b is filled somehow
		 * ((IPv4_Packet*)b.p.ip_packet)->get_dest_ip();
		 */
		class protocol_layers {} p;
	private:
		explicit SendBuffer(); // private constructor
		SendBuffer(const SendBuffer& s); // private copy constructor
		inline void* getDataStart() {return ((char*)this) + sizeof(SendBuffer);}
};

} // namespace ipstack
