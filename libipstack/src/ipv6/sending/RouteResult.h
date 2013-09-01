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
#include "ipv6/IPv6AddressUtilities.h"

namespace ipstack
{
	class Interface;
	class AddressEntry;
/**
 * Represents a result for a ipv6_find_route() call.
 */
struct RouteResult {
	/**
	 * The address of this address entry is recommended
	 * to be used as src address. The pointer may be 0 if interface is 0 or
	 * if the destination address was a multicast address.
	 */
	AddressEntry* proposed_src_addr;
	Interface* interface;
	/**
	 * If this route uses a default router instead of a prefix-matching
	 * router, this flag is set.
	 */
	bool isDefaultRouter;
	
	RouteResult() : proposed_src_addr(0), interface(0) {}
	RouteResult(AddressEntry* p,Interface* i) : proposed_src_addr(p), interface(i), isDefaultRouter(false) {}
};
} //namespace ipstack
