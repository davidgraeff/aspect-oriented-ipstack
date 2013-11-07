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

#include "ARP_IPv4_Packet.h"
#include "arp/ARP_Packet.h"


namespace ipstack {

class IPv4ToLinkLayer {
	public:
	/**
	* Look up an IP address and return the corresponding entry of the NDP cache.
	* @return Return a pointer to the entry or 0 if not found.
	*/
	NDPCacheEntry* find(const uint32_t ipv4_addr, uint8_t prefixlen=0, bool is_router = false);

	/**
	* May be influenced by the NDP_Neighbor_Solicitation aspect. If we do not use
	* static entries only, the neighbour discovery protocol will find out destination link
	* layer addresses for us. A request is send out if NDP_Neighbor_Solicitation is configured.
	* @param addr The IPv6 address
	* @param hwaddr The link layer address that belongs to the IPv6 address. If you add an entry
	* with an INCOMPLETE state you may set this 0.
	* @param state Set the state of the entry, If the entry already exist the state will be updated
	* to the given value.
	*/
	NDPCacheEntry* update(const uint32_t ipv4_addr, uint8_t* hwaddr, NDPCacheEntry::StateEnum state);
};

} // namespace ipstack
