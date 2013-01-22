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

#include "ipstack/router/Interface.h"
#include "ipstack/ipv6/IPv6AddressUtilities.h"
#include "ipstack/ipv6/IPv6onSockets.h"
#include "ipstack/SendBuffer.h"
#include "ipstack/icmpv6/ICMPv6.h"
#include "ipstack/icmpv6/ICMPv6_Socket.h"
#include "util/types.h"
#include "NDPMessages.h"
namespace ipstack
{
/**
 * Purpose: This class defines some commen functionality and defines
 * all structs for NDP messages.
 */
class NDPNeighborMessages
{
public:

	// Neighbor Solicitation message
	struct NeighborSolicitationMessage {
		UInt8 type; // 135
		UInt8 code; // 0
		UInt16 checksum;
		UInt32 reserved;
		
		ipv6addr target_address;
		UInt8 options[]; // Source link-layer address
	} __attribute__((packed));
	
	// Neighbor Advertisement message
	struct NeighborAdvertisementMessage {
		UInt8 type; // 136
		UInt8 code; // 0
		UInt16 checksum;
		
		UInt8 flags; // |R|S|O|  Reserved | //R: Router; S: Response to NeighborSolicitationMessage; O: Override cache entry
		UInt8 reserved1;
		UInt16 reserved2;
		
		ipv6addr target_address;
		UInt8 options[]; // Source link-layer address
		
		inline bool isRouter() {return 0x80 & flags;}
		inline bool isResponse() {return 0x40 & flags;}
		inline bool isOverride() {return 0x20 & flags;}
		inline void setRouter(bool v) {if (v) flags |= 0x80;}
		inline void setResponse(bool v) {if (v) flags |= 0x40;}
		inline void setOverride(bool v) {if (v) flags |= 0x20;}
	} __attribute__((packed));

	
	/**
	  * Prepare a neighbor_solicitation message and send it. You have to provide the src and dest ipv6 addresses
	  * and if the destination address of the actual ipv6 packet should be the solicitedNode multicast address of
	  * the destination address.
	  */
	static void send_neighbor_solicitation(const ipstack::ipv6addr& ipv6_srcaddr, const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface, bool useSolicitatedMulticastAddress);
};

} //namespace ipstack

