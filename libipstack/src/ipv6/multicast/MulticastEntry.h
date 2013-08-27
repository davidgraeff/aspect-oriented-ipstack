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
#include "util/types.h"

namespace ipstack {
/**
 * An IPv6->Link layer address cache. The link layer address have to be smaller or equal to 6 bytes in size.
 * Without further influences by aspects this implementation is synchronous: The process flow is blocked
 * while looking up a link layer address.
 */
struct MulticastGroupEntry {
	enum { ID = 240};
	unsigned char id;
	ipv6addr ipv6;
	UInt8 state;
	/**
	 * Use this after getting a free memory block from AddressMemory.
	 * This will mark the memory block as beeing used.
	 */
	void reserve() {
		id = ID;
	}
} __attribute__((packed));
#define MulticastGroupEntryType MulticastGroupEntry, MulticastGroupEntry::ID

}
