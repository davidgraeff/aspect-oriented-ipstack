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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "util/types.h"


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
		UInt32 dst_ipv4_addr;
		
		
		/**
		* IP sequence number (fragmentation etc)
		*/
		UInt16 id;
		
		void resolveRoute();
		
		void setupHeader(IPv4_Packet* packet, unsigned datasize);
		
		bool hasValidInterface() ;
		
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
		
		void set_dst_addr(UInt32 dst);
		void set_dst_addr(UInt8 a, UInt8 b, UInt8 c, UInt8 d);
		
		UInt32 get_dst_addr() ;
		
		UInt32 get_src_addr() ;
		
		bool hasValidSrcDestAddresses() ;
	}; // end IPV4 class
} //namespace ipstack
