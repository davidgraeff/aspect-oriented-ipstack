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
#include "util/protocol_layers.h"
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
		ReceiveBuffer(Interface* interface, char* data, uint16_t receivedSize);
		
		/**
		 * Return a pointer to the payload data.
		 */
		inline void* get_payload_data() {return payload;}

		/**
		 * Return the size in bytes of the payload, according to the length information
		 * that was stored together with the payload.
		 */
		inline uint16_t get_payload_size() {return m_payloadSize;}

		/**
		 * Return the size in bytes of the remaining storage
		 */
		inline uint16_t get_remaining_size() {return m_receivedSize-(payload-m_data);}
		
		/**
		 * Contains pointer to protocols. Example:
		 * ReceiveBuffer b;
		 * // b is filled somehow
		 * b.p.ipv4_packet->get_dest_ip();
		 */
		class protocol_layers {} p;
	private:
		ReceiveBuffer(const ReceiveBuffer& sp) {} // no copies
		ReceiveBuffer& operator= (const ReceiveBuffer&) {} // no assignements
		inline void* getDataStart() {return ((char*)this) + sizeof(SendBuffer);}
};

} // namespace ipstack

