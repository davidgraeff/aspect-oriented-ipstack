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

#include "ipstack/ipv6/IPv6AddressUtilities.h"
#include "util/types.h"

#include "../cfAttribs.h"

namespace ipstack
{

/**
 * An IPv6->Link layer address cache. Because NDP is link layer agnostic your
 * link layer have to weave in some members. 
 *
 * The link layer address and address size have to be weaved in by an aspect.
 * If you develop a link layer you have to weave in the following two members:
 * - enum {LINK_LAYER_ADDRESS_SIZE = 6; } // for a 6 byte link layer address
 * - UInt8 hw[LINK_LAYER_ADDRESS_SIZE];
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
	UInt8 state;
	
	// router state
	union {
		UInt8 router_state;
		struct {
		UInt8 isRouter:1;
		UInt8 isRouting:1;
		} __attribute__((packed));
	};
} __attribute__((packed));
#define NDPCacheEntryType NDPCacheEntry, NDPCacheEntry::ID

}