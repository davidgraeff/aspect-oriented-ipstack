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

#include "NDPRouterMessages.h"
#include "demux/Demux.h"

#include "icmpv6/ICMPv6_Packet.h"
#include "icmpv6/ICMPv6_Socket.h"
#include "ipv6/IPv6_Packet.h"
#include "ipv6/ndpcache/NDPCacheEntry.h"
#include "ipv6/AddressMemory.h"
#include "ipv6/IPv6.h"
#include "ip/SendbufferIP.h"

namespace ipstack {

void NDPRouterMessages::send_router_solicitation(ipstack::ipv6addr ipv6_dstaddr, Interface* interface) {

	ICMPv6_Socket &socket = ICMPv6_Socket::instance();
	IPV6& ipv6 = socket.ipv6;
	ipv6.set_dst_addr(ipv6_dstaddr, interface); // all routers multicast address
	// For the src address try to get the link local address
	if (interface->getInterfaceIPv6AddressByScope(ipv6_dstaddr, IPV6AddressScope::IPV6_SCOPE_LINKLOCAL))
		ipv6.set_src_addr(ipv6_dstaddr); // link-local
	else {
		get_unspecified_ipv6_address(ipv6_dstaddr);
		ipv6.set_src_addr(ipv6_dstaddr); // we have no link-local address, use the unspecified address
	}
	
	// Warning: We support ethernet only at the moment, because we reserving 8 additional bytes (2 bytes reserved) -> 6 bytes remaining for link layer address
	const uint8_t resSize = sizeof(NDPRouterMessages::RouterSolicitationMessage)
		+ NDPMessages::multiple_of_octets(interface->getAddressSize())*8;
	
	SendBuffer* sbi = SendbufferIP::requestIPBuffer(socket, socket, resSize);
	if (sbi) {
		sbi->mark("send_router_solicitation");
		NDPRouterMessages::RouterSolicitationMessage* msg = (NDPRouterMessages::RouterSolicitationMessage*)sbi->getDataPointer();
		msg->type = (133);
		msg->code = (0);
		msg->reserved = 0;
		NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer,(char*)msg->options, interface->getAddress(), interface->getAddressSize());
		sbi->writtenToDataPointer(resSize);
		sbi->send();
	}
}

void NDPRouterMessages::update_router_entry(Interface* interface) {


	// HINT: We do not take the Reachable Time and Retrans Timer into account for Neighbor Unreachability Detection.
	// The ndp cache expire_value field would have be 8 times that big per entry to do it standard compliant.
	// We assume for an embedded network stack: The topology will not change that often (and: the ndp cache refresh
	// time is configurable for fast changing topologies)
	if (foundMac) // Just set the expire_value to a reasonable value (if mac is known)
		interface->update_NDPCacheEntry_importance(entry, NDPCacheEntry::IMPORTANCE_router);

	// Default router?
	// We do not remember this value -> The router has to send another Router_Advertisement
	// if it want us to stop using it as a default router: Selfish but saves memory.
	entry->isRouting = is_routing;
	
	// Ok, now update (and add) all prefixes
	uint8_t routerPosition = interface->addressmemory.getPosition(entry);

	// First: Flag all entries of this router as ToBeDeleted
	uint8_t nextEntry = 0;
	while (AddressEntryIPv6* AddressEntryIPv6 = interface->ipv6.getAddress(&nextEntry)) {
		if (AddressEntryIPv6->routerEntryPosition == routerPosition) {
			AddressEntryIPv6->toBeRemoved = 1;
		}
	}


	// Second: Add/Update entries and remove ToBeDeleted flag
	if (!router_is_shuting_down) {
		NDPRouterMessages::OptionPrefixInformation* routerprefix = NDPRouterMessages::get_prefix_option(prefix_options, payloadlen);
		while (routerprefix) {
			if (routerprefix->useForStatelessAddressConfiguration()) {
				// Find ipv6 address with a prefixlen of this interface that equals the new propagated prefix
				AddressEntryIPv6* AddressEntryIPv6 = 0;
				while ((AddressEntryIPv6 = interface->ipv6.getAddress(&nextEntry))) {
					if (AddressEntryIPv6->routerEntryPosition == routerPosition &&
							compare_ipv6_addr(AddressEntryIPv6->ipv6, routerprefix->prefix, routerprefix->prefix_length)) {
						break;
					}
				}

				if (!AddressEntryIPv6) {
					AddressEntryIPv6 = interface->addAddress(routerprefix->prefix, routerprefix->prefix_length,
								AddressEntryIPv6::AddressEntryIPv6StateTemporary,
								routerPosition);
				}
				if (AddressEntryIPv6) {
					AddressEntryIPv6->toBeRemoved = 0;
					AddressEntryIPv6->hoplimit = hoplimit;
					AddressEntryIPv6->isOnLink = routerprefix->isOnLink();
					AddressEntryIPv6->valid_time_minutes = routerprefix->get_valid_lifetime();
					AddressEntryIPv6->preferred_time_minutes = routerprefix->get_preferred_lifetime();
				}
			}

			// next prefix
			payloadlen -= sizeof(NDPRouterMessages::OptionPrefixInformation);
			routerprefix = NDPRouterMessages::get_prefix_option((char*)routerprefix, payloadlen);
		}
	}

	// Third: Remove all flaged entries
	nextEntry = 0;
	while (AddressEntryIPv6* AddressEntryIPv6 = interface->ipv6.getAddress(&nextEntry)) {
		if (AddressEntryIPv6->routerEntryPosition == routerPosition && AddressEntryIPv6->toBeRemoved) {
			interface->ipv6.freeEntry(AddressEntryIPv6);
		}
	}
}

} //namespace ipstack

