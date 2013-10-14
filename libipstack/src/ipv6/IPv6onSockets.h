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
#include "IPv6AddressUtilities.h"


namespace ipstack {

class Interface;
class AddressEntry;
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
	 * Set the destination address. If
	 * interface is set, src address and hoplimit are not determined automatically! You
	 * should avoid setting the interface manually in most cases, but only if
	 * you set the the src address and hoplimit manually, too.
	 */
	void set_dst_addr(const ipv6addr& dst, Interface* interface = 0) ;

	/**
	 * If you really want to set the src ip yourself,
	 * instead of letting the interface choose one for you,
	 * you have to call this method after set_dst_addr!
	 */
	void set_src_addr(const ipv6addr& src) ;

	// Warning, this is a pointer to the address. Use memcpy
	// to replicate the address.
	const ipv6addr& get_dst_addr() const { return dst_ipv6_addr; }
	const ipv6addr& get_src_addr() const { return src_ipv6_addr; }
	
	/**
	 * Return the destination address as a next hop address in the basic implementation.
	 * If you enable the destination cache it will return a real on-link destination
	 * or the unspecified address if no on-link destination can be resolved.
	 */
	ipv6addr get_nexthop_ipaddr() ;

	bool hasValidSrcDestAddresses() const;
public:
	// cache for fast sending
	Interface* interface;
	// this is a copy of the most matching assigned ip of the interface
	// Because ipv6 may assign multiple source ips to an interface this is neccessary.
	// The best matching source ip will be assigned while setting the destination ip.
	ipv6addr src_ipv6_addr;
	friend class Router; // allow to call resolveRoute()
private:
	ipv6addr dst_ipv6_addr;
	uint8_t hoplimit;
	IPV6(const ipstack::IPV6 &) {} // Bug in aspectc++: Segfault if no copy constructur

	/**
	 * Determine source ip address and interface that fits the destination address set.
	 *
	 * @return resolveRoute will return an AddressEntry that contains an IPv6 address that is currently
	 * assigned to this interface. This IPv6 address fits best to the destination address the
	 * user has set. For example:
	 * We have currently assigned two addresses to this interface:
	 * - fe80::1:2:3:4:5:6
	 * - 2001::1:2:3:4:5:6
	 * If we want to send to the destination address fe80::7:8:9:a:b:c resolveRoute will return
	 * an addressEntry with fe80::1:2:3:4:5:6 because that address fits best to the destination address.
	 *
	 * The src address will be set accordingly. If you do not want resolveRoute to be called by set_dst_addr
	 * you have to set the second parameter "interface".
	 */
	AddressEntry* resolveRoute();

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

}; // end IPV6 class
} //namespace ipstack
