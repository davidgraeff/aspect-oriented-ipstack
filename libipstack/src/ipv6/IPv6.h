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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"
#include "router/Interface.h"
#include "ipv6/IPv6AddressUtilities.h"
#include "sending/RouteResult.h"


namespace ipstack {

class Interface;
class IPv6_Packet;
/**
 * Adds IPv6 functionality to a socket.
 */
class IPV6 {
public:
	IPV6() ;
	~IPV6() {}
	
	/**
	 * Call this after a requestBuffer call to reset all configuration
	 * you set like setting the hoplimit or activating special IPv6 headers.
	 */
	void restoreDefaultSendingConfiguration() ;

	void setHoplimit(uint8_t h) ;

	/**
	 * Set the destination address and determines the hoplimit, src_addr, interface
	 * and nexthop entry. If no nexthop entry from the neighbour cache can be resolved,
	 * nexthop will be set to NoEntry and ARP/NDP_ND will be executed when you're going
	 * to send a packet.
	 * 
	 * @param interface Usually the first matching interface for destination IP is
	 * used. For a link-local IP that may always be the first interface. If you want
	 * to send to a different interface, you have to provide it here.
	 */
	void set_dst_addr(const ipv6addr& dst, Interface* interface = 0);
	
	/**
	 * Reset cached next hop. This is called by set_dst_addr to find a
	 * new fitting next hop entry in the neighbor cache. It is also called
	 * by the Neighbor Discovery Protocol Redirect feature to actually change
	 * the nexthop from a router entry to the on-link host entry.
	 */
	void resetRoute();

	/**
	 * You can only choose from one of the source IP addresses, which
	 * are assigned to the current interface. The current interface is
	 * determined by set_dst_addr.
	 */
	void set_src_addr(AddressEntryIPv6* src_addr_entry) ;

	// Warning, this is a pointer to the address. Use memcpy
	// to replicate the address.
	const ipv6addr& get_dst_addr() const { return dst_ipv6_addr; }
	const ipv6addr get_src_addr() const {
		if (!src_addr_entry) {
			ipv6addr src_ipv6_addr;
			get_unspecified_ipv6_address(src_ipv6_addr);
			return src_ipv6_addr;
		} else
			return src_addr_entry->ipv6;
	}
	
	/**
	 * Return the IP addr of the next hop (router IP, on-link IP or destination IP).
	 * The next hop is determined in set_dst_addr.
	 */
	ipv6addr get_nexthop_ipaddr() ;

	bool hasValidSrcDestAddresses() const;
public:
private:
	// Src ip address (indirect, via AddressEntryIPv6)
	AddressEntryIPv6* src_addr_entry;
	// Dest ip address
	ipv6addr dst_ipv6_addr;
	// Next hop entry (point to neighbor cache entry for the
	// Dest IP, or a router, or is set to MulticastEntry)
	NeighbourCache::EntryPosition nexthop_destination;
	
	uint8_t hoplimit;
	IPV6(const ipstack::IPV6 &) {} // Bug in aspectc++: Segfault if no copy constructur

	/**
	 * Determine source ip address and interface that fits the destination address set.
	 *
	 * @return resolveRoute will return an AddressEntryIPv6 that contains an IPv6 address that is currently
	 * assigned to this interface. This IPv6 address fits best to the destination address the
	 * user has set. For example:
	 * We have currently assigned two addresses to this interface:
	 * - fe80::1:2:3:4:5:6
	 * - 2001::1:2:3:4:5:6
	 * If we want to send to the destination address fe80::7:8:9:a:b:c resolveRoute will return
	 * an AddressEntryIPv6 with fe80::1:2:3:4:5:6 because only that address fits to the destination address.
	 *
	 * The src address will be set accordingly. If you do not want resolveRoute to be called by set_dst_addr
	 * you have to set the second parameter "interface" (for multicast packets for example).
	 */
	AddressEntryIPv6* resolveRoute();

	void setupHeader(IPv6_Packet* packet, unsigned datasize) ;

	bool hasValidInterface() const;

	/**
	 * Return the specific size for a generated header with the currently
	 * set options of this object.
	 *
	 * Currently there is nothing that expands the IPv6 Header size.
	 * Just return IPV6_HEADER_SIZE_IN_BYTES.
	 *
	 * Extension headers may add size by aspects.
	 */
	unsigned getSpecificHeaderSize() ;

	/**
	 * Return true if this entry is useful for a route
	 * May be influenced by other aspects to check validity (For example the Time_Expire aspect).
	 */
	static bool find_route_is_matching(const ipv6addr& ipv6_dstaddr, AddressEntryIPv6* entry);

	/**
	 * Check each interface and all assigned IPv6 prefixes. If no fitting prefix was found
	 * a default router entry is returned. If no default router is known, an invalid RouteResult
	 * is returned.
	 * This method also proposes a fitting IPv6 src address to be used for the given destination address.
	 * (An IPv6 interface may have several IPv6 addresses assigned simultanously)
	 */
	RouteResult find_route(const ipv6addr& ipv6_dstaddr);
}; // end IPV6 class
} //namespace ipstack
