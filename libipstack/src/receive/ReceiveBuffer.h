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
#include "memory_management/MemoryInterface.h"
#include <string.h> //for memcpy

namespace ipstack
{

/**
 * Public API for inspecting received data by UDP/RawIP_Socket. Usually you 
 * use get_payload_data and get_payload_size. If you are interested in the
 * remote host information like remote ip address or remote udp port, you may
 * use the protocol_pointer member variable.
 */
class ReceiveBuffer : public ReceiveBufferPrivate
{
	public:
		ReceiveBuffer(char* data, uint16_t receivedSize);
		
		/**
		 * Return a pointer to the payload data.
		 * */
		inline void* get_payload_data() {return payload;}

		/**
		 * Return the size in bytes of the received data
		 * */
		inline uint16_t get_payload_size() {return m_receivedSize-(payload-m_data);}

		/**
		 * Contains pointer to protocols. Example:
		 * ReceiveBuffer b;
		 * // b is filled somehow
		 * v.protocol_pointer.ipv4->get_dest_ip();
		 */
		class protocol_pointer_extensions {} protocol_pointer;
	protected:
		uint16_t m_receivedSize;
		// This is either a pointer to the network hardwares receive buffer, or if you have used
		// ::clone() it is a pointer to a memory block of a socket (and points to m_cloned_data).
		char* m_data;
		// is true if ::cloned() have been used
		bool cloned;
		// payload pointer (points to the last set protocol pointer)
		char* payload;
		// if cloned has been used the packet data starts here
		char m_cloned_data[];

};

} // namespace ipstack

