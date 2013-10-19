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

#include "ipv6/IPv6AddressUtilities.h"
#include "util/ipstack_inttypes.h"

#include "cfAttribs.h"

namespace ipstack
{

/**
 * An IPv6->Link layer address cache. Because NDP is link layer agnostic your
 * link layer have to weave in some members. 
 *
 * The link layer address and address size have to be weaved in by an aspect.
 * If you develop a link layer you have to weave in the following two members:
 * - enum {LINK_LAYER_ADDRESS_SIZE = 6; } // for a 6 byte link layer address
 * - uint8_t hw[LINK_LAYER_ADDRESS_SIZE];
 * 
 * Synchronous: Without further influence by an aspect this implementation is synchronous:
 * The process flow is blocked while looking up a link layer address.
 *
 * Expire: Without further influences by aspects an ndp cache entry may never expire.
 */
struct NDPCacheEntry {
	/**
	 * The first member is always "id".
	 * Use reserve() after getting a free memory block from AddressMemory.
	 * This will mark the memory block as beeing used.
	 */
	// 
	enum { ID = 250};
	unsigned char id;
	void reserve() {
		id = ID;
	}
	
	// ipv6 address
	ipv6addr ipv6;

	// state
	enum StateEnum { NDPCacheEntryState_Incomplete, NDPCacheEntryState_Stale, NDPCacheEntryState_Delay,
	NDPCacheEntryState_Probe, NDPCacheEntryState_Reachable
	};
	uint8_t state;
	
	// router state
	union {
		uint8_t router_state;
		struct {
		uint8_t isRouter:1;
		uint8_t isRouting:1;
		} __attribute__((packed));
	};
} __attribute__((packed));
#define NDPCacheEntryType NDPCacheEntry, NDPCacheEntry::ID

}