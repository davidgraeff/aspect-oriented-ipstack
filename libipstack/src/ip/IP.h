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
// Copyright (C) 2013 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"
#include "util/LinkedList.h"

namespace ipstack
{

	/**
	* Inherit from this class if you want your socket to be extended by the API specific to
	* the internet protocol (IP). Some generic methods are always available, ipv4/ipv6 related
	* methods are introduced by aspects.
	*/
	class IP {
	public:
		/**
		 * Return true if src and dest address are set up in the
		 * used ip version.
		 * Extended by aspects.
		 */
		bool hasValidSrcDestAddresses() const { return false; }
		
		Interface* getUsedInterface() { return 0; }
		
		// 4 bits of the first byte are for the version information of an IP packet
		static inline uint8_t get_ip_version(char* packet) {
			union {
			  uint8_t first_byte;
			  uint8_t other:4, version:4;
			};
			first_byte = packet[0];
			return version;
		
		/**
		 * @return Return true if the src addr of the @ip_packet is matching with
		 * the destination addr in ipv4/ipv6.
		 * This method is used by TCP to match connections.
		 */
		bool is_ip_peer_addr_matching(char* ip_packet) { return false; }
		
		/**
		 * @return Return true if the src addr of this socket and of @ip is matching and
		 * if this socket and @ip are using the same IP versions.
		 * This method is used by TCP to bind a connection and ensure uniqueness.
		 */
		bool is_ip_src_addr_matching(IP& ip) { return false; }
		
		/**
		 * Set the ipv4/ipv6 destination addr to the the src address of the ip packet.
		 * This method is used by TCP to remember the peer IP address.
		 */
		void set_dst_ip_addr_to_peer_addr(char* ip_packet) {}
	};
} // namespace ipstack
