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
// Copyright (C) 2012 David Gräff

#pragma once

#include "util/types.h"

namespace ipstack
{
class ICMPv6_Packet
{
	public:
		enum { ICMP_HEADER_SIZE = 4,
			//ICMP_MAX_DATA_SIZE = 576-20-ICMP_HEADER_SIZE, // echo data can be large ... but not that large :-)
			ICMP_MAX_DATA_SIZE = 56, //dafault values: linux: 56, windows: 32 ... hopefully 56 will be sufficient
			IPV6_TYPE_ICMP = 58,

			// info messages (/rfc4443#section-4)
			ICMP_TYPE_ECHO_REQUEST = 128,
			ICMP_CODE_ECHO_REQUEST = 0,
			ICMP_TYPE_ECHO_REPLY = 129,
			ICMP_CODE_ECHO_REPLY = 0
		};
		enum {
			ICMP_TYPE_PARAMETER_PROBLEM = 4,
			ICMP_CODE_PARAMETER_PROBLEM_HEADERFIELD = 0,
			ICMP_CODE_PARAMETER_PROBLEM_UNKNOWN_NEXTHEADER = 1,
			ICMP_CODE_PARAMETER_PROBLEM_UNKNOWN_OPTION = 2
		};
		enum {
			// error messages (/rfc4443#section-3)
			ICMP_TYPE_DESTINATION_UNREACHABLE = 1,
			ICMP_CODE_PORT_UNREACHABLE = 4,
			ICMP_TYPE_PACKET_TOO_BIG = 2,
			ICMP_CODE_PACKET_TOO_BIG = 0,
			ICMP_TYPE_TIME_EXCEEDED = 3
		};

	private:
		UInt8 type;
		UInt8 code;
		UInt16 checksum;
		UInt8 data[];

	public:
		UInt8 get_type() {
			return type;
		}
		void set_type(UInt8 t) {
			type = t;
		}

		UInt8 get_code() {
			return code;
		}
		void set_code(UInt8 c) {
			code = c;
		}

		UInt16 get_checksum() {
			return checksum;
		}
		void set_checksum(UInt16 csum) {
			checksum = csum;
		}

		UInt8* get_data() {
			return data;
		}

} __attribute__((packed));

} //namespace ipstack
