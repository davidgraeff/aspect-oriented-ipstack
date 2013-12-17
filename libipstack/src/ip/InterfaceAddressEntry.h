 
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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"

namespace ipstack {
	
/**
 * An IP address, assigned to an interface. If this is an entry, not assigned by
 * a router (dhcp, NDP) but manually, the routerEntryPosition is "EntryUndefined".
 *
 * Initially an IPv6 entry is flagged as temporary until a
 * neighbour address duplication check has been performed. The entry will either
 * be dismissed (duplicate found) or the temporary flag will be removed.
 * 
 * Multicast addresses are handled different, especially for resolving the link layer
 * address, therefore we flag multicast addresses.
 * 
 * The link layer address of an on-link IP addresses always have to be able to be resolved
 * (no router entry allowed!).
 */
class AddressEntry {
public:
	uint8_t prefixlen; // 0 means entry is not used!
	
	union {
		uint8_t stateflag;
		struct {
			uint8_t isEnabled:1;
			uint8_t isTemporary:1;
			uint8_t isMulticast:1;
			uint8_t isOnLink:1;
			uint8_t toBeRemoved:1;
		} __attribute__((packed));
	};
	
	uint8_t routerEntryPosition;
	uint8_t hoplimit;
} __attribute__((packed));

} //namespace ipstack
