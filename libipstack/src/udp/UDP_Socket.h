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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "memory_management/SocketMemory.h"
#include "demux/DemuxLinkedListContainer.h"
#include <string.h>

namespace ipstack
{

class UDP_Packet;
/**
 * UDP Socket. You will see only a few API methods here in the code. Sending/Receiving is added per
 * aspect slice.
 */
class UDP_Socket : public UDP_Socket_Private, public DemuxLinkedList<UDP_Socket>, public SocketMemory
{
	public:
		// Construct with socket memory
		UDP_Socket(const SocketMemory& memory);
		~UDP_Socket();
		
		/// Destination port
		void set_dport(uint16_t d);
		uint16_t get_dport();

		/// Source port
		void set_sport(uint16_t s);
		uint16_t get_sport();

		/**
		 * Bind this socket to the source port you have set before.
		 * You do not need to call unbind/bind to set a new source port.
		 */
		bool bind();
		/**
		 * Unbind this socket from the source port you have set before.
		 * This is called automatically on destruction of the socket. 
		 * You do not need to call unbind/bind to set a new source port.
		 */
		void unbind();
};

} //namespace ipstack
