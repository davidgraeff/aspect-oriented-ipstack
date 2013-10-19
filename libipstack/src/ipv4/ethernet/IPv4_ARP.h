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
#include <string.h> //for memcpy
#include "util/ipstack_inttypes.h"
#include "ethernet/Eth_Frame.h"
#include "router/Interface.h"
#include "os_integration/Clock.h"

#include "Eth_ARP_IPv4_Packet.h"
#include "ARP_Packet.h"


namespace ipstack {

class IPv4_ARP {
	public:
	// Lookup the corresponding ethernet address for an ipv4 address.
	// Uses the neighbour cache for lookups.
	// If not found, send ARP Requests and wait for replies. This call is
	// blocking.
	static const uint8_t* lookup(uint32_t ipv4_addr, Interface* interface);
	
	// handle incoming arp request: return an arp reply packet to the requesting host 
	static void reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, ReceiveBuffer& b);
	
	// Handle incoming arp replies.
	// Uses the neighbour cache as storage
	static void update(const uint32_t ipv4_addr, const uint8_t* hw_addr);
	
	// Send an ARP-Request packet
	static void request(uint32_t ipv4_addr, Interface* interface);
	
	private:
	
	// Lookup the corresponding ethernet address for an ipv4 address.
	// Uses the neighbour cache for lookups. Returns 0 if not found.
	static uint8_t* find(uint32_t ipv4_addr);
};

} // namespace ipstack
