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


#include "IPv6ToLinkLayer.h"
#include "ip/neighbour_cache/NeighbourCacheConfig.h"
#include "arp/ARPConfig.h"

namespace ipstack {
	/**
	* Look up an IP address and return the corresponding entry of the NDP cache.
	* In contrast to ipv6_lookup this method will not send a neighbour solicitation message.
	* @return Return a pointer to the entry or 0 if not found.
	*/
	NDPCacheEntry* ndpcache_find(const ipv6addr& addr) {
		uint8_t nextEntry = 0;
		while (NDPCacheEntry* entry = addressmemory.findEntry<NDPCacheEntryType>(&nextEntry)) {
			if (compare_ipv6_addr(addr, entry->ipv6)) {
				return entry;
			}
		}
		return 0;
	}
	NDPCacheEntry* findIPv6Router() {
		uint8_t nextEntry = 0;
		while (NDPCacheEntry* entry = addressmemory.findEntry<NDPCacheEntryType>(&nextEntry)) {
			if (entry->isRouting && entry->state.ip6) {
				return entry;
			}
		}
		return 0;
	}
	
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
	NDPCacheEntry* update(ipv6addr addr, uint8_t* hwaddr, NDPCacheEntry::StateEnum state) {
		// Look for a duplicate and return this instead. Update the entry state.
		NDPCacheEntry* entry = ndpcache_find(addr);
		if (entry) {
			entry->state = state;
			return entry;
		}

		entry = (NDPCacheEntry*)addressmemory.findEntry<FreeEntryType>();
		// The cache is full: Try to release an NDP Cache entry
		if (!entry) {
			dropUnused();
			entry = (NDPCacheEntry*)addressmemory.findEntry<FreeEntryType>();
		}
		// No success -> abort and be unhappy :/
		if (!entry)
			return 0;

		// copy ip address, link layer address and state
		copy_ipv6_addr(addr, entry->ipv6);
		if (hwaddr) memcpy(entry->hw, hwaddr, sizeof(entry->hw));
		entry->state = state;
		
		// make the entry valid actually
		reserveNDPCacheEntry(entry);
		return entry;
	}
} // namespace ipstack
