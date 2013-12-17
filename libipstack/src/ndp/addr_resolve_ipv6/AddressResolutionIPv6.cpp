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
#include "ip/neighbour_cache/NeighbourCache.h"
#include "ip/neighbour_cache/NeighbourEntry.h"
#include "ipv6/ndpcache/NDPCacheConfig.h"
#include "ipv6/IPv6AddressUtilities.h"
#include "ipv6/AddressMemory.h"
#include "ipv6/IPv6_Packet.h"
#include "ipv6/IPv6.h"
#include "os_integration/Clock.h"
#include <string.h>

namespace ipstack {

	NeighbourEntry* AddressResolutionIPv6::lookup(const ipv6addr& addr, Interface* interface) {
		NeighbourEntry* entry = interface->ndpcache_find(ip->get_dst_ipaddr());
		if (entry == 0) {
			entry = interface->addNeighbourEntry(ip->get_dst_ipaddr(), 0, NeighbourEntry::NeighbourEntryState_Incomplete);

			if (entry) {
				// Wait for ndp reply
				uint64_t waitTicks = Clock::ms_to_ticks(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
				uint64_t timeout = Clock::now() + waitTicks;
				while(Clock::now() < timeout) {
					socket->block(waitTicks);
					hw_addr = find(ipv4_addr);
					if (is_NeighbourEntry_reachable) return entry;
				}

				interface->block_until_neighbor_advertisement(entry);
				// Check state
				if (entry->state != NeighbourEntry::NeighbourEntryState_Reachable) {
					interface->addressmemory.freeEntry(entry);
					entry = interface->ndpcache_findRouter();
				}
			} else {
				// Not enough memory. Try a router instead
				entry = findRouter();
			}
		}
	}

} // end namespace