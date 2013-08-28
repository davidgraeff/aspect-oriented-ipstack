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
// Copyright (C) 2011 Christoph Borchert


#ifndef __INTERNET_CHECKSUM__
#define __INTERNET_CHECKSUM__

#include "IPv4.h"
#include <inttypes.h>

namespace ipstack {

class InternetChecksumV4 {
	public:
	static inline uint16_t byteswap16(uint16_t val) {
		return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	}
	static uint32_t computePseudoHeader(IPv4_Packet* packet, uint16_t payloadlen, uint8_t upper_layer_nextheader){
		uint32_t csum = payloadlen; //length of (udp, tcp) frame
	
		csum += upper_layer_nextheader; // protocol
	
		uint32_t src = packet->get_src_ipaddr();
		csum += byteswap16(src); // source address (1)
		csum += byteswap16(src>>16); // source address (2)

		uint32_t dst = packet->get_dst_ipaddr();
		csum += byteswap16(dst); // destination address (1)
		csum += byteswap16(dst>>16); // destination address (2)
	
		return csum;
	}
};

} //namespace ipstack

#endif // __INTERNET_CHECKSUM__ 

