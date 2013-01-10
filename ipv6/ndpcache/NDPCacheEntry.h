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
 * An IPv6->Link layer address cache. The link layer address have to be smaller or equal to 6 bytes in size.
 * Without further influences by aspects this implementation is synchronous: The process flow is blocked
 * while looking up a link layer address.
 */
struct NDPCacheEntry {
	enum { ID = 250};
	unsigned char id;
	ipv6addr ipv6;
	UInt8 hw[6];
	enum StateEnum { NDPCacheEntryState_Incomplete, NDPCacheEntryState_Stale, NDPCacheEntryState_Delay,
	NDPCacheEntryState_Probe, NDPCacheEntryState_Reachable
	};
	UInt8 state;
	enum NDPCacheEntryImportance { IMPORTANCE_not_important = 0, IMPORTANCE_good_to_know = 50, IMPORTANCE_solicitation_renew = 70, IMPORTANCE_router = 100, IMPORTANCE_important = 128, IMPORTANCE_always_keep = 255 };
	UInt8 expire_value; // the purpose of this byte is defined by the strategy that is used to expire values
	union {
		UInt8 router_state;
		struct {
		UInt8 isRouter:1;
		UInt8 isRouting:1;
		} __attribute__((packed));
	};
	/**
	 * Use this after getting a free memory block from AddressMemory.
	 * This will mark the memory block as beeing used.
	 */
	void reserve() {
		id = ID;
	}
	/**
	 * This struct may be expanded by an aspect to allow asynchronous lookups:
	 * A pointer to the Sendbuffers is set here and the frame will be send as
	 * soon as the link layer address is known and the process flow is not
	 * blocked during the lookup.
	 */
	//UInt8 retry_time_seconds;
	//UInt8 tries;
	//SendBuffer* data_pointer;
} __attribute__((packed));
#define NDPCacheEntryType NDPCacheEntry, NDPCacheEntry::ID

}