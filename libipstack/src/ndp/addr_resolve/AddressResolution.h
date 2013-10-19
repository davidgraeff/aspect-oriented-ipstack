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
#include "ipv6/IPv6AddressUtilities.h"
#include "ipv6/AddressMemory.h"
#include "ipv6/IPv6_Packet.h"
#include "ipv6/IPv6onSockets.h"
#include "os_integration/Clock.h"
#include <string.h>

namespace ipstack {

	class IPv6_NDP
	{
		private:
			bool is_NDPCacheEntry_reachable(NDPCacheEntry* entry) {
				return (entry->state == NDPCacheEntry::NDPCacheEntryState_Reachable);
			}
			
			NDPCacheEntry* findRouter() {
				uint8_t nextEntry = 0;
				while (NDPCacheEntry* entry = addressmemory.findEntry<NDPCacheEntryType>(&nextEntry)) {
					if (entry->isRouting) {
						return entry;
					}
				}
				return 0;
			}
			
			void request(const ipv6addr& addr, Interface* interface) {
				
			}
			
			NDPCacheEntry* lookup(const ipv6addr& addr, Interface* interface) {
				NDPCacheEntry* entry = interface->ndpcache_find(ip->get_dst_ipaddr());
				if (entry == 0) {
					entry = interface->addNDPCacheEntry(ip->get_dst_ipaddr(), 0, NDPCacheEntry::NDPCacheEntryState_Incomplete);

					if (entry) {
						// Wait for ndp reply
						uint64_t waitTicks = Clock::ms_to_ticks(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
						uint64_t timeout = Clock::now() + waitTicks;
						while(Clock::now() < timeout) {
							socket->block(waitTicks);
							hw_addr = find(ipv4_addr);
							if (is_NDPCacheEntry_reachable) return entry;
						}

						interface->block_until_neighbor_advertisement(entry);
						// Check state
						if (entry->state != NDPCacheEntry::NDPCacheEntryState_Reachable) {
							interface->addressmemory.freeEntry(entry);
							entry = interface->ndpcache_findRouter();
						}
					} else {
						// Not enough memory. Try a router instead
						entry = findRouter();
					}
				}
			}
			
	const uint8_t* ARP_Cache::lookup(uint32_t ipv4_addr, Interface* interface){
		uint8_t* hw_addr = find(ipv4_addr);
		if (hw_addr) return hw_addr;
		
		for(uint8_t retries=0; retries < ARP_REQUEST_ATTEMPTS; ++retries){
			// Request
			ipv4_request(ipv4_addr, interface);
			// Wait for arp packet
			uint64_t waitTicks = Clock::ms_to_ticks(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
			uint64_t timeout = Clock::now() + waitTicks;
			while(Clock::now() < timeout) {
				block(waitTicks);
				hw_addr = find(ipv4_addr);
				if (hw_addr) return hw_addr;
			}
		}
		return hw_addr;
	}

		public:
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

			void freeNDPCacheEntry(NDPCacheEntry* entry) {
				uint8_t pos = addressmemory.getPosition(entry);
				if (pos == AddressMemory::EntryUndefined)
					return;
				if (entry->isRouter) {
					// Free all address prefixes that belong to this router
					uint8_t nextEntry = 0;
					while (AddressEntry* addressentry = addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
						if (addressentry->routerEntryPosition == pos)
							addressmemory.freeEntry(addressentry);
					}
				}
				addressmemory.freeEntry(entry);
			}
			
			/**
			* If an entry has been confirmed by external means (static or by NDP Neighbor Advertisement)
			* this method is called. This can be used by aspects to for example reset the expire time.
			* The basic implementation does nothing.
			*/
			void entryConfirmed(NDPCacheEntry* entry) {
				
			}
			
			/**
			* An aspect has to weave in a strategy to regain space in the NDP cache.
			*/
			void dropUnused() {
				
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
			NDPCacheEntry* addNDPCacheEntry(ipv6addr addr, uint8_t* hwaddr, NDPCacheEntry::StateEnum state) {
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
			
			/**
			* The given AddressMemory entry is currently not marked as an NDPCache Entry.
			* An entry->reserve() call is necessary. We set some default values here, too.
			*/
			void reserveNDPCacheEntry(NDPCacheEntry* entry) {
				entry->router_state = 0;
				entry->reserve();
			}
	};
} // end namespace