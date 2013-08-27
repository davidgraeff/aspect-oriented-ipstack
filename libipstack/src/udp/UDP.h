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


#ifndef __UDP__
#define __UDP__

#include "util/types.h"
#include "ip/InternetChecksum.h"

namespace ipstack {

class UDP_Packet{
	public:
	enum { UDP_HEADER_SIZE = 8,
			IP_TYPE_UDP = 17, //same for IPv4/IPv6
			UNUSED_PORT = 0 }; // http://www.iana.org/assignments/port-numbers

	private:
	UInt16 sport;
	UInt16 dport;
	UInt16 length;
	UInt16 checksum;
	UInt8 data[];
	
	public:
	UInt16 get_sport() const { return sport; }
	void set_sport(UInt16 s) { sport = s; }
	
	UInt16 get_dport() const { return dport; }
	void set_dport(UInt16 d) { dport = d; }
	
	UInt16 get_length() const { return length; }
	void set_length(UInt16 len) { length = len; }
	
	UInt16 get_checksum() const { return checksum; }
	void set_checksum(UInt16 csum) { checksum = csum; }
	/**
	  * Calculate the udp checksum.
	  * @param sum The pseudo header sum
	  * @param payloadlen We use extern information about the size of this packet
	  * @param The interface on which the packet will be send. Necessary for checksum offloading
	  */
	UInt16 calc_checksum(UInt32 csum, UInt16 payloadlen, Interface* interface) {
		csum += InternetChecksum::computePayload((UInt8*)this, payloadlen);
		return ~InternetChecksum::accumulateCarryBits(csum); // one's complement
	}
	bool checksum_valid(UInt32 csum, UInt16 payloadlen, Interface* interface) {
		return calc_checksum(csum, payloadlen, interface) == 0;
	}
			
	UInt8* get_data() { return data; }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif // __UDP__
