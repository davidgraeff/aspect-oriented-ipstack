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

#pragma once

#include "util/ipstack_inttypes.h"
#include "ip/InternetChecksum.h"

namespace ipstack
{

class ICMPv4_Packet
{
	public:
		enum { ICMP_HEADER_SIZE = 8,
			   //ICMP_MAX_DATA_SIZE = 576-20-ICMP_HEADER_SIZE, // echo data can be large ... but not that large :-)
			   ICMP_MAX_DATA_SIZE = 56, //dafault values: linux: 56, windows: 32 ... hopefully 56 will be sufficient
			   IPV4_TYPE_ICMP = 1,
			   ICMP_TYPE_DESTINATION_UNREACHABLE = 3,
			   ICMP_CODE_PROTOCOL_UNREACHABLE = 2,
			   ICMP_CODE_PORT_UNREACHABLE = 3,
			   ICMP_TYPE_ECHO_REQUEST = 8,
			   ICMP_TYPE_ECHO_REPLY = 0,
			   ICMP_CODE_ECHO = 0,
			   ICMP_TYPE_SOURCE_QUENCH = 4,
			   ICMP_CODE_SOURCE_QUENCH = 0
			 };

	private:
		uint8_t type;
		uint8_t code;
		uint16_t checksum;
		uint32_t quench;
		uint8_t data[];

	public:
		uint8_t get_type() {
			return type;
		}
		void set_type(uint8_t t) {
			type = t;
		}

		uint8_t get_code() {
			return code;
		}
		void set_code(uint8_t c) {
			code = c;
		}

		uint16_t get_checksum() {
			return checksum;
		}
		void set_checksum(uint16_t csum) {
			checksum = csum;
		}
		/**
		  * Calculate the icmpv4 checksum.
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
	
		uint32_t get_quench() {
			return quench;
		}
		void set_quench(uint32_t q) {
			quench = q;
		}

		uint8_t* get_data() {
			return data;
		}

} __attribute__((packed));

} //namespace ipstack
