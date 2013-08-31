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
#include "ipv6/IPv6AddressUtilities.h"
#include <string.h>

namespace ipstack {

class IPv6hc_Packet {
public:
	enum { ETH_TYPE_IPV6hc = 0x86DE, //There is no IPv6 6lowpan over ethernet specified. This is for debugging purposes only.
		   HC_HEADERSIZE_WITHOUT_FRAG_HEADER = 2, HC_FRAG_HEADER = 4
		 };

private:
	// for the fragment header
	// no initalizing, no little/big endian. This number is just random and increases with every packet
	static uint16_t datagram_tag_counter;
	
	char fragmentheader[2];
	uint16_t datagram_tag;
	uint8_t data[];

	// four bytes
	void writeFragmentHeader(uint16_t size) ;


	// aspect weaves in ethernet src addr
	void get_src_ip_from_upper_protocol_addr(ipv6addr& mem) {
	}
	// aspect weaves in ethernet dest addr
	void get_dest_ip_from_upper_protocol_addr(ipv6addr& mem) {
	}
public:
	void setUpHeader() ;
	uint16_t get_payload_len() ;
	void set_payload_len(uint16_t len) ;
	uint8_t* get_payload() ;
	static uint8_t getWriteSrcHeaderSize() {return 16;}
	static uint8_t getWriteDestHeaderSize() {return 16;}
	static uint8_t getWriteMulticastDestHeaderSize() {return 16;}
	static uint8_t getWriteHeaderSize() ;
	uint8_t calculateHCHeaderSizeWithoutFragmentSize() ;
	uint8_t calculateHeaderSize() ;

	uint8_t get_hoplimit() ;
	void set_hoplimit(uint8_t t) ;

	uint8_t get_nextheader() ;
	void set_nextheader(uint8_t i) ;

	ipv6addr get_src_ipaddr() ;

	void set_src_ipaddr(const ipv6addr& addr) ;

	ipv6addr get_dst_ipaddr() ;
	void set_dst_ipaddr(const ipv6addr& addr) ;
private:
	ipv6addr read_compressed_ipaddr(ipv6addr& mem, uint8_t* readPtr, uint8_t flagbyte) ;
	ipv6addr read_compressed_multicast_dest_ipaddr(ipv6addr& mem, uint8_t* readPtr, uint8_t flagbyte) ;

	void write_compressed_src_ipaddr(const ipv6addr& addr, uint8_t* writePtr, uint8_t& flagbyte) ;
	void write_compressed_dst_ipaddr(const ipv6addr& addr, uint8_t* writePtr, uint8_t& flagbyte) ;
	void write_compressed_multicast_dst_ipaddr(const ipv6addr& addr, uint8_t* writePtr, uint8_t& flagbyte) ;

} __attribute__((packed));  //__attribute__ ((aligned(1), packed));

} //namespace ipstack
