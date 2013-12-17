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

#include "Multicast.h"

namespace ipstack {
	/**
		* Return true if the given address is a solicitedNode multicast address (the multicast address
		* that correspond to a link local address)
		*/
	bool Multicast_IPv6::is_solicitedNode_multicast_addr(const ipstack::ipv6addr& multicast_addr) {
		if (multicast_addr.ipaddrB8[0] == 0xff && multicast_addr.ipaddrB8[1] == 0x02) {
			uint8_t nextEntry = 0;
			while (AddressEntryIPv6* entry = addr->getAddress(&nextEntry)) {
				const ipv6addr& ip = entry->ipv6;
				if (ip.ipaddrB8[13] ==  multicast_addr.ipaddrB8[13] && ip.ipaddrB8[14] ==  multicast_addr.ipaddrB8[14] &&
					ip.ipaddrB8[15] ==  multicast_addr.ipaddrB8[15]) {
					return true;
				}
			}
		}
		return false;
	}
	
	/**
		* Return true if the given multicast address is already assigned to this interface
		*/
	bool Multicast_IPv6::multicast_addr_joined(const ipstack::ipv6addr& multicast_addr) {
		uint8_t nextEntry = 0;
		while (MulticastGroupEntry* entry = addr->getAddress(&nextEntry)) {
			if (compare_ipv6_addr(entry->ipv6, multicast_addr)) {
				return true;
			}
		}
		return false;
	}

	/**
		* Join the multicast group of the solicitedNode multicast address
		*/
	void Multicast_IPv6::join_solicitedNode_multicastGroup(const ipstack::ipv6addr& ip_addr) {
		ipstack::ipv6addr solicitedNode;
		solicitedNode_multicast_addr(ip_addr, solicitedNode);
		join_multicastGroup_solicitedNode(solicitedNode);
	}
	/**
		* Leave the multicast group of the solicitedNode multicast address
		*/
	void Multicast_IPv6::leave_solicitedNode_multicastGroup(const ipstack::ipv6addr& ip_addr) {
		ipstack::ipv6addr solicitedNode;
		solicitedNode_multicast_addr(ip_addr, solicitedNode);
		leave_multicastGroup(solicitedNode);
	}
	
	/**
		* Join the multicast group of the link local all-routers multicast address. The IPv6 RFC
		* require an implementation to register to the all-routers multicast address as soon
		* as the interface gets up.
		*/
	void Multicast_IPv6::join_linkLocal_allRouters_multicastGroup() {
		ipstack::ipv6addr multicast_addr = {{0}};
		multicast_addr.ipaddrB8[0] = 0xff;
		multicast_addr.ipaddrB8[1] = 0x02;
		multicast_addr.ipaddrB8[15] = 0x02;
		join_multicastGroup(multicast_addr, false);
	}
	/**
		* Join the multicast group of the link local all-nodes multicast address. Traffic destinated to
		* all link neighbours may be send to this address (NDP address resolution etc). The IPv6 RFC
		* require an implementation to register to the all-nodes multicast address as soon
		* as the interface gets up.
		*/
	void Multicast_IPv6::join_linkLocal_allNodes_multicastGroup() {
		ipstack::ipv6addr multicast_addr = {{0}};
		multicast_addr.ipaddrB8[0] = 0xff;
		multicast_addr.ipaddrB8[1] = 0x02;
		multicast_addr.ipaddrB8[15] = 0x01;
		join_multicastGroup(multicast_addr, false);
	}
	
	void Multicast_IPv6::leave_linkLocal_allRouters_multicastGroup() {
		ipstack::ipv6addr multicast_addr = {{0}};
		multicast_addr.ipaddrB8[0] = 0xff;
		multicast_addr.ipaddrB8[1] = 0x02;
		multicast_addr.ipaddrB8[15] = 0x02;
		leave_multicastGroup(multicast_addr);
	}
	void Multicast_IPv6::leave_linkLocal_allNodes_multicastGroup() {
		ipstack::ipv6addr multicast_addr = {{0}};
		multicast_addr.ipaddrB8[0] = 0xff;
		multicast_addr.ipaddrB8[1] = 0x02;
		multicast_addr.ipaddrB8[15] = 0x01;
		leave_multicastGroup(multicast_addr);
	}

	/**
		* Leave a multicast group. Send MLD message. This method is used by all specific methods above
		* and may be used to explicitly leave a specific multicast group.
		*/
	void Multicast_IPv6::leave_multicastGroup(const ipstack::ipv6addr& multicast_addr) {
		// remove if found
		uint8_t nextEntry = 0;
		while (MulticastGroupEntry* entry = addr->getAddress(&nextEntry)) {
			if (compare_ipv6_addr(entry->ipv6, multicast_addr)) {
				addr->freeEntry(entry);
				return;
			}
		}
	}

	/**
		* Join a multicast group. Send MLD message. This method is used by all specific methods above
		* and may be used to explicitly join a specific multicast group.
		*/
	bool Multicast_IPv6::join_multicastGroup_solicitedNode(const ipstack::ipv6addr& multicast_addr, bool registerToRouters = true) {return true;}
	bool Multicast_IPv6::join_multicastGroup              (const ipstack::ipv6addr& multicast_addr, bool registerToRouters = true) {
		if (multicast_addr_joined(multicast_addr))
			return false;
		
		MulticastGroupEntry* entry = addr->addAddress(multicast_addr, 64, AddressEntryIPv6::AddressEntryIPv6StateMulticast);
		return (entry != 0);
	}
}
