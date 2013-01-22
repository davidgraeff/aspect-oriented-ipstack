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

/**
  * A sendbuffer is used for all sending activity within the tcp/ip stack. It is manipulated and filled
  * by different aspects of the network stack layers. The size for the buffer has to be known beforehand
  * to allocate the memory. YOu may use different methods to write to a sendbuffer. See the documentation
  * at Socket_Send_Slice.ah.
  */
class SendBuffer
{
	public:
		/**
		  * Create a new SendBuffer object and allocate memory for it. If not enough memory is available 0
		  * is returned. You should provide the destination interface. It is recommend to use the convenience
		  * methods of yout socket instead of a raw SendBuffer directly.
		  */
		static SendBuffer* createInstance(Mempool* mempool, UInt16Opt requestedSize, Interface* interface) {
			SendBuffer* r = (SendBuffer*)mempool->alloc(requestedSize + sizeof(SendBuffer));
			if (!r)
				return 0;
// 			r->m_mempool = mempool;
			r->m_memsize = requestedSize;
			r->m_state = WritingState;
			r->m_interface = interface;
			r->initStartPointer();
			return r;
		}
		
		/**
		  * We use a trick here to determine where to write the user data. We compute the the size of the class
		  * and determine the end of the object in the memory. This has the advantage that other aspects may
		  * add data members to this class and we will still begin with the user data after those statically
		  * added member variables.
		  */
		void initStartPointer() {
			data = getDataStart();
		}
		inline void* getDataStart() {
			return this + sizeof(SendBuffer);
		}
		
		/**
		  * For debugging purposes you may associate a string with this sendbuffer. You have to enable
		  * the SendBuffer Debuging feature for an actual output. If not enabled all mark methods in the
		  * code should be optimised out by the compiler (only use const char strings!).
		  * @param str The string to be used for debug messages.
		  */
		inline void mark(const char* str) {
			
		}

		/**
		 * Put your data here.
		 * Example:
		 * SendBuffer* buffer = socket.requestSendBuffer(4);
		 * memcpy(buffer->getDataPointer(), "test", 4);
		 * OR
		 * *((int32*)buffer->getDataPointer()) = 1472984;
		 * Always call writtenToDataPointer after writting to this memory location!
		 */
		void* getDataPointer() {
			return data;
		}

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
			UInt16Opt availableLength = getRemainingSize();
			if (availableLength < length)
				length = availableLength;

			memcpy(data, newdata, length);
			data = (char*)data + length;
		}

		/**
		  * After allocating a SendBuffer it is in the
		  * "Writing-State". After calling send(..) with the buffer it is in the
		  * "Transmitted-State". If an aspect fails on the buffer it will set it
		  * to the "Invalid-State".
		  *
		  * You may only write to a buffer if it is in the "Writing-State". Calling
		  * send(..) is also only a valid action in this state. A buffer should never
		  * be free'd while in the "Transmitted-State" without calling hasBeenSend()
		  * before. Use the socket API for correct usage (socket::free)!
		  *
		  * If you write an aspect that intercept send(..) (to delay packets etc), you
		  * may use the "AboutToBeTransmitted-State" to mark a buffer as ready to
		  * be send.
		  */
		enum {InvalidState= 0x00, WritingState = 0x01, AboutToBeTransmittedState= 0x02, TransmittedState = 0x04, ResolveLinkLayerStateOption= 0x10};
		inline void setState(UInt8 s) { m_state = s; }
		inline UInt8 getState() { return m_state;}
		inline UInt8 getStateWithoutOptions() { return m_state & 0x0f;} // lower bits only
		
		/**
		  * A sendbuffer may be recycled if no negative effects can occur anymore.
		  * E.g. after the network driver has successfully send the content of this
		  * buffer (DMA finished).
		  */
		void recycle() {
			m_state = WritingState;
		}

		/**
		 * Return the size in bytes that is available for your data.
		 * Cache this value!
		 * */
		UInt16Opt getRemainingSize() {
			return m_memsize - ((char*) data - (char*) getDataStart());
		}
		UInt16Opt getSize() {
			return m_memsize;
		}
		/**
		* Return true if the sendbuffer has been send already. This will also return true if the generation of the sendbuffer
		* failed and the sendbuffer therefore never have been relayed to the network buffer. Therefore do not
		* call this method with failed-to-generate sendbuffers!
		*/
		bool hasBeenSend() {
			return m_interface->hasBeenSent(getDataStart());
		}
		inline Interface* getInterface() { return m_interface; }
		void setInterface(Interface* i) { m_interface = i; }
	private:
		explicit SendBuffer() {} // private constructor
		SendBuffer(const SendBuffer& s) {} // private copy constructor
		
// 		Mempool* m_mempool;
		UInt16Opt m_memsize; // set to  user requested size
		UInt8 m_state;
		Interface* m_interface;
		void* data;
};

} // namespace ipstack
