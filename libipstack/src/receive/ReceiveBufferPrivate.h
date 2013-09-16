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
			* b.clone(*udp_socket, b.protocol_pointer.ipv4);
			* // The last statement copies the receive buffer content from the receiving task
			* // to the udp_socket and omits the ethernet information (starting with ipv4)
			*/
			bool clone(SocketMemory& socket_mem, void* start_mem = 0);
	};

} // namespace ipstack

