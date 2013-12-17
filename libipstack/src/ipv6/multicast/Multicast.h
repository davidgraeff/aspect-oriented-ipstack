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

namespace ipstack {
/**
  * A few methods for joining and leaving multicast groups. An interface only accepts traffic for a multicast address
  * if the interface joined the corresponding multicast group. The MLD protocol will be used to additionally register 
  * for a multicast address on neighbour routers.
  */
class Multicast_IPv6 {
	private:
		InterfaceAddressesIPv6* addr;
	public:
		Multicast_IPv6(InterfaceAddressesIPv6* addr) : addr(addr) {}
		
		/**
		  * Return true if the given address is a solicitedNode multicast address (the multicast address
		  * that correspond to a link local address)
		  */
		bool is_solicitedNode_multicast_addr(const ipstack::ipv6addr& multicast_addr);
		
		/**
		  * Return true if the given multicast address is already assigned to this interface
		  */
		bool multicast_addr_joined(const ipstack::ipv6addr& multicast_addr);

		/**
		  * Join the multicast group of the solicitedNode multicast address
		  */
		void join_solicitedNode_multicastGroup(const ipstack::ipv6addr& ip_addr);
		/**
		  * Leave the multicast group of the solicitedNode multicast address
		  */
		void leave_solicitedNode_multicastGroup(const ipstack::ipv6addr& ip_addr);
		
		/**
		  * Join the multicast group of the link local all-routers multicast address. The IPv6 RFC
		  * require an implementation to register to the all-routers multicast address as soon
		  * as the interface gets up.
		  */
		void join_linkLocal_allRouters_multicastGroup();
		/**
		  * Join the multicast group of the link local all-nodes multicast address. Traffic destinated to
		  * all link neighbours may be send to this address (NDP address resolution etc). The IPv6 RFC
		  * require an implementation to register to the all-nodes multicast address as soon
		  * as the interface gets up.
		  */
		void join_linkLocal_allNodes_multicastGroup();
		
		void leave_linkLocal_allRouters_multicastGroup();
		void leave_linkLocal_allNodes_multicastGroup();

		/**
		 * Leave a multicast group. Send MLD message. This method is used by all specific methods above
		 * and may be used to explicitly leave a specific multicast group.
		 */
		void leave_multicastGroup(const ipstack::ipv6addr& multicast_addr);

		/**
		 * Join a multicast group. Send MLD message. This method is used by all specific methods above
		 * and may be used to explicitly join a specific multicast group.
		 */
		bool join_multicastGroup_solicitedNode(const ipstack::ipv6addr& multicast_addr, bool registerToRouters = true);
		bool join_multicastGroup              (const ipstack::ipv6addr& multicast_addr, bool registerToRouters = true);
};

}