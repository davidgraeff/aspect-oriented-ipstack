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

#include <inttypes.h>


namespace ipstack {
	class Interface;
	class IPv4_Packet;
	/**
	 * Adds IPv4 functionality to a socket.
	 */
	class IPV4 {
	public:
		// cache for fast sending
		Interface* interface;
		bool using_gateway;
	private:
		uint32_t dst_ipv4_addr;
		
		
		/**
		* IP sequence number (fragmentation etc)
		*/
		uint16_t id;
		
		void resolveRoute();
		
		void setupHeader(IPv4_Packet* packet, unsigned datasize);
		
		bool hasValidInterface() const;
		
		/**
		 * Return the specific size for a generated header with the currently
		 * set options of this object.
		 * 
		 * Currently there is nothing that expands the IPv4 Header size.
		 * Just return IPV4_MIN_HEADER_SIZE.
		 */
		unsigned getSpecificHeaderSize() ;

	public:
		IPV4() ;
		
		void set_dst_addr(uint32_t dst);
		void set_dst_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
		
		uint32_t get_dst_addr() const;
		
		uint32_t get_src_addr() const;
		
		uint32_t get_nexthop_ipaddr();
		
		bool hasValidSrcDestAddresses() const;
	}; // end IPV4 class
} //namespace ipstack
