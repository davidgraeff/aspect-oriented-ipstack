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

#include "util/types.h"
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
	static UInt16 datagram_tag_counter;
	
	char fragmentheader[2];
	UInt16 datagram_tag;
	UInt8 data[];

	// four bytes
	void writeFragmentHeader(UInt16 size) ;


	// aspect weaves in ethernet src addr
	void get_src_ip_from_upper_protocol_addr(ipv6addr& mem) {
	}
	// aspect weaves in ethernet dest addr
	void get_dest_ip_from_upper_protocol_addr(ipv6addr& mem) {
	}
public:
	void setUpHeader() ;
	UInt16 get_payload_len() ;
	void set_payload_len(UInt16 len) ;
	UInt8* get_payload() ;
	static UInt8 getWriteSrcHeaderSize() {return 16;}
	static UInt8 getWriteDestHeaderSize() {return 16;}
	static UInt8 getWriteMulticastDestHeaderSize() {return 16;}
	static UInt8 getWriteHeaderSize() ;
	UInt8 calculateHCHeaderSizeWithoutFragmentSize() ;
	UInt8 calculateHeaderSize() ;

	UInt8 get_hoplimit() ;
	void set_hoplimit(UInt8 t) ;

	UInt8 get_nextheader() ;
	void set_nextheader(UInt8 i) ;

	ipv6addr get_src_ipaddr() ;

	void set_src_ipaddr(const ipv6addr& addr) ;

	ipv6addr get_dst_ipaddr() ;
	void set_dst_ipaddr(const ipv6addr& addr) ;
private:
	ipv6addr read_compressed_ipaddr(ipv6addr& mem, UInt8* readPtr, UInt8 flagbyte) ;
	ipv6addr read_compressed_multicast_dest_ipaddr(ipv6addr& mem, UInt8* readPtr, UInt8 flagbyte) ;

	void write_compressed_src_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte) ;
	void write_compressed_dst_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte) ;
	void write_compressed_multicast_dst_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte) ;

} __attribute__((packed));  //__attribute__ ((aligned(1), packed));

} //namespace ipstack
