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

#include "IPv6.h"
#include "../router/Interface.h"
#include "../ip/InternetChecksum.h"
#include "util/types.h"

namespace ipstack
{

class InternetChecksumV6
{
	public:
		static UInt32 computePseudoHeader(IPv6_Packet* packet, UInt16 payloadlen, UInt8 upper_layer_nextheader) {
			UInt32 csum = payloadlen; //length of (udp, tcp) frame (without IPv6 additional headers)

			// Source ip
			ipv6addr ipaddr = packet->get_src_ipaddr();
			csum += (ipaddr.ipaddrB8[ 1] + (UInt16)(ipaddr.ipaddrB8[ 0] << 8));
			csum += (ipaddr.ipaddrB8[ 3] + (UInt16)(ipaddr.ipaddrB8[ 2] << 8));
			csum += (ipaddr.ipaddrB8[ 5] + (UInt16)(ipaddr.ipaddrB8[ 4] << 8));
			csum += (ipaddr.ipaddrB8[ 7] + (UInt16)(ipaddr.ipaddrB8[ 6] << 8));
			csum += (ipaddr.ipaddrB8[ 9] + (UInt16)(ipaddr.ipaddrB8[ 8] << 8));
			csum += (ipaddr.ipaddrB8[11] + (UInt16)(ipaddr.ipaddrB8[10] << 8));
			csum += (ipaddr.ipaddrB8[13] + (UInt16)(ipaddr.ipaddrB8[12] << 8));
			csum += (ipaddr.ipaddrB8[15] + (UInt16)(ipaddr.ipaddrB8[14] << 8));
			
			// Dest ip
			ipaddr = packet->get_dst_ipaddr();
			csum += (ipaddr.ipaddrB8[ 1] + (UInt16)(ipaddr.ipaddrB8[ 0] << 8));
			csum += (ipaddr.ipaddrB8[ 3] + (UInt16)(ipaddr.ipaddrB8[ 2] << 8));
			csum += (ipaddr.ipaddrB8[ 5] + (UInt16)(ipaddr.ipaddrB8[ 4] << 8));
			csum += (ipaddr.ipaddrB8[ 7] + (UInt16)(ipaddr.ipaddrB8[ 6] << 8));
			csum += (ipaddr.ipaddrB8[ 9] + (UInt16)(ipaddr.ipaddrB8[ 8] << 8));
			csum += (ipaddr.ipaddrB8[11] + (UInt16)(ipaddr.ipaddrB8[10] << 8));
			csum += (ipaddr.ipaddrB8[13] + (UInt16)(ipaddr.ipaddrB8[12] << 8));
			csum += (ipaddr.ipaddrB8[15] + (UInt16)(ipaddr.ipaddrB8[14] << 8));
			
			csum += upper_layer_nextheader; // we do not use packet->get_nextheader() because we want the upper layer
			// next header and not the ipv6 next header (may be an ipv6 header extension)
			
			return csum;
		}
		static UInt16 compute(IPv6_Packet* packet, UInt16 payloadlen, Interface* interface, UInt8 upper_layer_nextheader, void* payload) {
			UInt32 csum = computePseudoHeader(packet, payloadlen, upper_layer_nextheader);
			csum += InternetChecksum::computePayload((UInt8*)payload, payloadlen, interface);
			return InternetChecksum::invert(InternetChecksum::accumulateCarryBits(csum), interface); // one's complement
		}
		
		static bool valid(IPv6_Packet* packet, UInt16 payloadlen, Interface* interface, UInt8 upper_layer_nextheader, void* payload) {
			return (compute(packet, payloadlen, interface, upper_layer_nextheader, payload) == 0);
		}

};

} //namespace ipstack

