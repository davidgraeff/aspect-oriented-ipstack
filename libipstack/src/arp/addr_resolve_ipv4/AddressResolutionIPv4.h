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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff

#pragma once
#include "util/ipstack_inttypes.h"

namespace ipstack {

class NeighbourEntry;
class Interface;
class AddressResolutionIPv4 {
	public:
	// Lookup the corresponding ethernet address for an ipv4 address.
	// Uses the neighbour cache for lookups.
	// If not found, send ARP Requests and wait for replies. This call is
	// blocking.
	static inline const NeighbourEntry* lookup(uint32_t ipv4_addr, Interface* interface);

	// Send an ARP-Request packet
	static inline void request(uint32_t ipv4_addr, Interface* interface);
};

} // namespace ipstack
