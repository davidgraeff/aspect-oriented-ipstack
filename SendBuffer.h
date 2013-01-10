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

#include "util/Mempool.h"
#include "router/Interface.h"
#include <string.h> //for memcpy
#include "util/types.h"
#include "stdio.h"
namespace ipstack
{

class SendBuffer
{
	private:
// 		char m_dataStart[]; // memory block starts here (but we have to add the size of this class for a memory pointer where data can be written)
	public:
		static SendBuffer* createSendBuffer(Mempool* mempool, UInt16Opt requestedSize) {
			SendBuffer* r = (SendBuffer*)mempool->alloc(requestedSize + sizeof(SendBuffer));
			if (!r)
				return 0;
			r->m_mempool = mempool;
			r->m_memsize = requestedSize;
			r->initStartPointer();
			return r;
		}
		
		// Initialize the data pointer
		// This is "aspect safe": even if aspects add other data members to this class, we will never overwrite those
		void initStartPointer() {
			data = getDataStart();
		}

		/**
		 * Put your data here.
		 * Example:
		 * SendBuffer* buffer = socket.requestSendBuffer(12);
		 * memcpy(buffer->data, "test", 4);
		 * OR
		 * (int32)(buffer->data[0]) = 1472984;
		 * Do not change the pointer address yourself!
		 * Always call writtenToDataPointer after writting to this memory location!
		 */
		void* data;

		/**
		 * Use this method to increment the data pointer after writing to it directly
		 */
		void writtenToDataPointer(UInt16Opt length) {
			data = (char*)data + length;
		}

		/**
		 * A convenient method for writing to this SendBuffer.
		 * This is less efficient if you incrementally compute data and aggregate those instead
		 * of directly write to the data pointer in incremental steps.
		 */
		void write(const void* newdata, UInt16Opt length) {
			if (!m_memsize) return;

			UInt16Opt availableLength = getRemainingSize();
			if (availableLength < length)
				length = availableLength;

			memcpy(data, newdata, length);
			data = (char*)data + length;
		}

		void makeInvalid() {
			m_memsize = 0;
		}

		bool isValid() {
			return (m_memsize != 0);
		}

		/**
		 * Return the size in bytes that is available for your data.
		 * Cache this value!
		 * */
		UInt16Opt getRemainingSize() {
			if (!this->m_memsize) return 0;
			return m_memsize - ((char*) data - (char*) getDataStart());
		}
		UInt16Opt getSize() {
			return m_memsize;
		}
		inline void* getDataStart() {
			return this + sizeof(SendBuffer);
		}

	public: //TODO private
		Mempool* m_mempool;
		UInt16Opt m_memsize; // set to  user requested size
};

/**
 * Some times it is neccary to carry the interface inline to ask a SendBuffer if it has been send already. This is the case for all
 * management sockets.
 */
class SendBufferWithInterface
{
public:
	static SendBufferWithInterface* createSendBuffer(Mempool* mempool, UInt16Opt requestedSize, Interface* interface) {
		SendBufferWithInterface* r = (SendBufferWithInterface*)mempool->alloc(requestedSize + sizeof(SendBufferWithInterface));
		if (!r)
			return 0;
		r->sendbuffer.m_mempool = mempool;
		r->sendbuffer.m_memsize = requestedSize;
		r->sendbuffer.initStartPointer();
		r->interface = interface;
		return r;
	}
	/**
	  * Return true if the sendbuffer has been send already. This will also return true if the generation of the sendbuffer
	  * failed (isValid()==false) and the sendbuffer therefore never have been relayed to the network buffer. Therefore do not
	  * call this method with failed-to-generate sendbuffers!
	  */
	bool hasBeenSend() {
		return !sendbuffer.isValid() && interface->hasBeenSent(sendbuffer.getDataStart());
	}
	Interface* interface;
	SendBuffer sendbuffer;
};

} // namespace ipstack
