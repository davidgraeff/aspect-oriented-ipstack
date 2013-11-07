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
// Copyright (C) 2013 David Gräff

#pragma once


#include "util/ipstack_inttypes.h"
#include "util/singleton.h"
#include "NeighbourEntry.h"

namespace ipstack
{

/**
 * 
 */
class NextHopUpdateTable : public Singleton<NextHopUpdateTable>
private:
	struct {
		NeighbourEntry* e;
		ipaddr a;
	} entry;
	entry entries[3];
	uint8_t updateSize;
public:
	void add(ipaddr, NeighbourEntry*);
// 				IPV6* ipv6instance = Router::Inst().getFirstDestinationCacheEntry();
// 			while (ipv6instance) {
// 				if (compare_ipv6_addr(ipv6instance->get_dst_addr(), message->destination_address)) {
// 					ipv6instance->set_nexthop_ndpcache_entry(ndpcacheentry);
// 				}
// 				ipv6instance = ipv6instance->getNext();
// 			}
	void checkUpdate(ipaddr);
};

}
