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
#include "router/Interface.h"

namespace ipstack
{

/**
 * An IP->Link layer address cache.
 *
 * The link layer address and address size have to be weaved in by an aspect.
 * If you develop a link layer you have to weave in the following two members:
 * - enum {LINK_LAYER_ADDRESS_SIZE = 6; } // for a 6 byte link layer address
 * - uint8_t hw[LINK_LAYER_ADDRESS_SIZE];
 *
 * Expire: Without further influences by aspects a neighbour cache entry may never expire.
 */
struct NeighbourEntry {
	// Weave in a link layer ...
	
	// Weave in IPv4/v6 address ...
	
	// state
	enum StateEnum {
		NeighbourEntryState_Incomplete, NeighbourEntryState_Stale, NeighbourEntryState_Delay,
		NeighbourEntryState_Probe, NeighbourEntryState_Reachable
	};
	union {
		uint8_t state8;
		struct {
		uint8_t reachable:4;
		uint8_t ip4:1;
		uint8_t ip6:1;
		uint8_t reserved:2;
		} __attribute__((packed));
	} state;
	
	// router state
	union {
		uint8_t router_state8;
		struct {
		uint8_t isRouter:1;
		uint8_t isRouting:1;
		uint8_t isStatic:1;
		} __attribute__((packed));
	} router_state;
	
	// Interface
	Interface* interface;
	
} __attribute__((packed));

}