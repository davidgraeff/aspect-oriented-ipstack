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
#include "ipv6/ndpcache/NDPCacheConfig.h"
#include "ipv6/ndpcache_expire/NDPCacheExpireConfig.h"
#include "ipv6/IPv6AddressUtilities.h"
#include "ipv6/AddressMemory.h"
#include "ipv6/IPv6_Packet.h"
#include "ipv6/IPv6.h"
#include "os_integration/Clock.h"
#include <string.h>

using namespace ipstack;

class ExpireStrategy_Usage
{
	public:
	// May be influenced by NDP_Router_Solicitation to not increase the expire_value but instead
	// wait for a router advertisement
	static void router_entry_refresh(NeighbourEntry* entry) {
		// Basic method: Just set the router entry to a higher importance
		// It would be better of couse, if we instead send out a NDP_Router_Solicitation
		entry->expire_value = NeighbourEntry::IMPORTANCE_router;
	}
	
	// May be influenced by NDP_Neighbor_Solicitation to not increase the expire_value but instead
	// wait for a neighbor advertisement
	static void nonrouter_entry_refresh(NeighbourEntry* entry) {
		// Basic method: Just set the entry to a higher importance
		// It would be better of couse, if we instead send out a NDP_Neighbour_Solicitation
		entry->expire_value = NeighbourEntry::IMPORTANCE_good_to_know;
	}

	/**
		* Always use this method if you use a cache entry anywhere.
		* This method make sure that a used entry will not be dismissed.
		* @param entry The ndp cache entry
		*/
	static void useentry(NeighbourEntry* entry) {
		// This entry is used. We have to make sure it will not be dismissed from the cache.
		// If the importance is very low, set it to a higher value
		if (entry->expire_value < NDPCACHE_TRESHOLD_REQUIRE_SOLICITATION) { // Feintuning
			if (entry->expire_value==1) // marked as "to be renewed"
				return;
			entry->expire_value = 1;
			if (entry->isRouter) {
				router_entry_refresh(entry);
			} else
				nonrouter_entry_refresh(entry);
		} else if (entry->expire_value & 1) {
			// The importance is already high. But at least
			// if the expire rate timer reduced the importance of this entry: increase it again.
			entry->expire_value += NDPCACHE_EXPIRE_RATE_PER_MINUTE - 1;
		}
	}

	/**
		* Kill all entries that have an importance of NDPCACHE_EXPIRE_RATE_PER_MINUTE or lower.
		* This method should be called regulary (~2min).
		* @return Return number of released entries
		*/
	static uint8_t releaseOldEntries() {
		uint8_t nextEntry = 0;
		uint8_t releasedEntries = 0;
		while (NeighbourEntry* entry = NeighbourCache.Inst().findEntry(&nextEntry)) {
			if (entry->expire_value < NDPCACHE_EXPIRE_RATE_PER_MINUTE) {
				freeNeighbourEntry(entry);
				++releasedEntries;
			} else if (entry->expire_value != 255) {
				// Lower the expire value and set the first bit to mark this
				// Do not decrease expire_values of 255. Those entries are fixed.
				entry->expire_value = (entry->expire_value - NDPCACHE_EXPIRE_RATE_PER_MINUTE) | 1;
			}
		}
		return releasedEntries;
	}

	/**
		* Release an entry that is older than the given value.
		* For example if the ndp cache is full, but we have to save an
		* important link layer address lookup.
		*/
	static void releaseEntryBelowImportance(uint8_t importance_below_value) {
		NeighbourEntry* oldest_entry = 0;

		uint8_t nextEntry = 0;
		while (NeighbourEntry* entry = NeighbourCache.Inst().findEntry(&nextEntry)) {
			if (entry->expire_value != 255 && entry->expire_value < importance_below_value) {
				importance_below_value = entry->expire_value;
				oldest_entry = entry;
			}
		}

		if (oldest_entry)
			freeNeighbourEntry(oldest_entry);
	}
	
	static void update_NeighbourEntry_importance(NeighbourEntry* entry, NeighbourEntry::NeighbourEntryImportance importance_level) {
		entry->expire_value |= importance_level; // minimum of (importance level and existing value)
	}
};
