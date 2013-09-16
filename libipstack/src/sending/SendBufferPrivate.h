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
#include <string.h> //for memcpy
#include "util/ipstack_inttypes.h"
namespace ipstack
{

/**
  * Non public API!
  */
class SendBufferPrivate
{
	public:
		/**
		  * We use a trick here to determine where to write the user data. We compute the the size of the class
		  * and determine the end of the object in the memory. This has the advantage that other aspects may
		  * add data members to this class and we will still begin with the user data after those statically
		  * added member variables.
		  */
		void initStartPointer();
		inline void* getDataStart() {return ((char*)this) + sizeof(SendBuffer);}
		
		/**
		  * For debugging purposes you may associate a string with this sendbuffer. You have to enable
		  * the SendBuffer Debuging feature for an actual output. If not enabled all mark methods in the
		  * code should be optimised out by the compiler (only use const char strings!).
		  * @param str The string to be used for debug messages.
		  */
		inline void mark(const char* str) {
			
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
		inline void setState(uint8_t s) { m_state = s; }
		inline uint8_t getState() { return m_state;}
		inline uint8_t getStateWithoutOptions() { return m_state & 0x0f;} // lower bits only
		
		inline Interface* getInterface() { return m_interface; }
		void setInterface(Interface* i);
		
		/**
		* Return true if the sendbuffer has been send already. This will also return true if the generation of the sendbuffer
		* failed and the sendbuffer therefore never have been relayed to the network buffer. Therefore do not
		* call this method with failed-to-generate sendbuffers!
		*/
		bool hasBeenSend();
	protected:
		uint_fast16_t m_memsize; // set to  user requested size
		uint8_t m_state;
		Interface* m_interface;
		void* data;
};

} // namespace ipstack
