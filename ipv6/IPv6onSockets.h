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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "util/types.h"
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
	// cache for fast sending
	Interface* interface;
	// this is a copy of the most matching ip of the interface
	// Because ipv6 may assign multiple source ips to an interface this is neccessary.
	// The best matching source ip will be assigned while setting the destination ip.
	// This is fully compliant to the specification and important to keep tcp connections on the same source ip.
	ipv6addr src_ipv6_addr;
	friend class Router; // allow to call resolveRoute()
private:
	ipv6addr dst_ipv6_addr;
	UInt8 hoplimit;
	IPV6(const ipstack::IPV6 &) {} // Bug in aspectc++: Segfault if no copy constructur

	/**
	 * Return the src-ip-prefix entry that belongs to the src-ip that has been determined
	 * after the destination was known.
	 * This is important to know for the destination-cache aspect to calculate the next-hop.
	 */
	AddressEntry* resolveRoute();

	/**
	 * Assume that all destinations are on-link and just
	 * return the destination address as next hop. This is influenced
	 * by the destination cache aspect.
	 */
	ipv6addr get_nexthop_ipaddr() ;

	void setupHeader(IPv6_Packet* packet, unsigned datasize) ;

	bool hasValidInterface() ;

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

public:
	IPV6() ;
	~IPV6() {}
	
	/**
	 * Call this after a requestBuffer call to reset all configuration
	 * you set like setting the hoplimit or activating special IPv6 headers.
	 */
	void restoreDefaultSendingConfiguration() ;

	void setHoplimit(UInt8 h) ;

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

	// Warning, this is a pointer to the address!
	const ipv6addr& get_dst_addr() const { return dst_ipv6_addr; }
	const ipv6addr& get_src_addr() const { return src_ipv6_addr; }

	bool hasValidSrcDestAddresses() ;
}; // end IPV6 class
} //namespace ipstack
