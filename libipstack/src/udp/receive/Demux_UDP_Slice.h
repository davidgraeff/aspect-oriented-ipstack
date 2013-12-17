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
/**
 * Purspose: add bind/unbind (specific to udp sockets) to demux. This
 * will introduce a head pointer for a linked list for binded sockets.
 */

#include "util/ipstack_inttypes.h"
#include "udp/UDP_Socket.h"
#include "udp/UDP_Packet.h"

using namespace ipstack;

slice class Demux_UDP_Slice
{
	private:
	/**
	* This method should be called if the udp port of the given socket is not assigned on the given
	* interface.
	*/
	void error_port_not_reachable(ReceiveBuffer& buffer) {}
};
