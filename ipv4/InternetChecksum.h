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
// Copyright (C) 2011 Christoph Borchert


#ifndef __INTERNET_CHECKSUM__
#define __INTERNET_CHECKSUM__

#include "IPv4.h"
#include "util/types.h"

namespace ipstack {

class InternetChecksumV4 {
	public:
	static inline UInt16 byteswap16(UInt16 val) {
		return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	}
	static UInt32 computePseudoHeader(IPv4_Packet* packet, UInt16 payloadlen, UInt8 upper_layer_nextheader){
		UInt32 csum = payloadlen; //length of (udp, tcp) frame
	
		csum += upper_layer_nextheader; // protocol
	
		UInt32 src = packet->get_src_ipaddr();
		csum += byteswap16(src); // source address (1)
		csum += byteswap16(src>>16); // source address (2)

		UInt32 dst = packet->get_dst_ipaddr();
		csum += byteswap16(dst); // destination address (1)
		csum += byteswap16(dst>>16); // destination address (2)
	
		return csum;
	}
};

} //namespace ipstack

#endif // __INTERNET_CHECKSUM__ 

