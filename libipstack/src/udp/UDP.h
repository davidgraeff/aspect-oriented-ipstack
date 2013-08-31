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

#include "util/ipstack_inttypes.h"
#include "ip/InternetChecksum.h"

namespace ipstack {

class UDP_Packet{
	public:
	enum { UDP_HEADER_SIZE = 8,
			IP_TYPE_UDP = 17, //same for IPv4/IPv6
			UNUSED_PORT = 0 }; // http://www.iana.org/assignments/port-numbers

	private:
	uint16_t sport;
	uint16_t dport;
	uint16_t length;
	uint16_t checksum;
	uint8_t data[];
	
	public:
	uint16_t get_sport() const { return sport; }
	void set_sport(uint16_t s) { sport = s; }
	
	uint16_t get_dport() const { return dport; }
	void set_dport(uint16_t d) { dport = d; }
	
	uint16_t get_length() const { return length; }
	void set_length(uint16_t len) { length = len; }
	
	uint16_t get_checksum() const { return checksum; }
	void set_checksum(uint16_t csum) { checksum = csum; }
	/**
	  * Calculate the udp checksum.
	  * @param sum The pseudo header sum
	  * @param payloadlen We use extern information about the size of this packet
	  * @param The interface on which the packet will be send. Necessary for checksum offloading
	  */
	uint16_t calc_checksum(uint32_t csum, uint16_t payloadlen, Interface* interface) {
		csum += InternetChecksum::computePayload((uint8_t*)this, payloadlen);
		return ~InternetChecksum::accumulateCarryBits(csum); // one's complement
	}
	bool checksum_valid(uint32_t csum, uint16_t payloadlen, Interface* interface) {
		return calc_checksum(csum, payloadlen, interface) == 0;
	}
			
	uint8_t* get_data() { return data; }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif // __UDP__
