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
// Copyright (C) 2012 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"
#include "memory_management/SocketMemory.h"
#include "router/Interface.h"

namespace ipstack
{

	/**
	 * Non public API for receiving code accross the ipstack.
	 */
	class ReceiveBufferPrivate
	{
		public:
			/**
			* Clone this receive buffer to the memory of the given socket.
			* You may also specify a start position to clone from, to exclude
			* for instance the ethernet protocol.
			* 
			* ________Example:
			* ReceiveBuffer b; // usually from demux
			* UDP_Socket* udp_socket; // usually also from demux
			* ReceiveBuffer* socket_receive_buffer = b.clone(*udp_socket, b.protocol_pointer.ipv4);
			* // The last statement copies the receive buffer content from the receiving task
			* // to the udp_socket and omits the ethernet information (starting with ipv4)
			*/
			ReceiveBuffer* clone(SocketMemory& socket_mem, void* start_mem = 0);
			
			inline Interface* get_interface() const { return m_interface; }
			inline void setPayload(char*& payload, uint16_t& payloadSize) {
				this->payload = payload;
				this->m_payloadSize = payloadSize;
			}
			
		protected:
			// We have to know the interface if we want to use this ReceiveBuffer for
			// a response that is directed to the source host.
			Interface* m_interface;
			// The complete size of the contained data (not just the payload data)
			uint16_t m_receivedSize;
			uint16_t m_payloadSize;
			// This is either a pointer to the network hardwares receive buffer, or if you have used
			// ::clone() it is a pointer to a memory block of a socket (and points to m_cloned_data).
			char* m_data;
			// is true if ::cloned() have been used
			bool cloned;
			// payload pointer (points to the last set protocol pointer)
			char* payload;
	};

} // namespace ipstack

