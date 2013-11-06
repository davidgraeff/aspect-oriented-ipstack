#include "IPv6onSockets.h"
#include "ipv6/IPv6_Packet.h"
#include "router/Router.h"
#include "RouteResult.h"

namespace ipstack {
	IPV6::IPV6() : interface(0) {
		get_unspecified_ipv6_address(src_ipv6_addr);
		get_unspecified_ipv6_address(dst_ipv6_addr);
		restoreDefaultSendingConfiguration();
	}

	ipv6addr IPV6::get_nexthop_ipaddr()
	{
		return dst_ipv6_addr;
	}

	void IPV6::setupHeader(IPv6_Packet* packet, unsigned int datasize)
	{
		packet->setUpHeader();
		packet->set_payload_len(datasize - IPv6_Packet::IPV6_HEADER_SIZE_IN_BYTES);
		packet->set_hoplimit(hoplimit);
		//packet->set_nextheader(protocol_id); // The next header will be set in a SendBuffer advice
		packet->set_src_ipaddr(get_src_addr()); // Use the get method
		packet->set_dst_ipaddr(get_dst_addr()); // Use the get method
	}

	bool IPV6::hasValidInterface() const { return (interface != 0); }

	unsigned int IPV6::getSpecificHeaderSize()
	{
		return IPv6_Packet::IPV6_HEADER_SIZE_IN_BYTES;
	}

	void IPV6::restoreDefaultSendingConfiguration()
	{
		hoplimit = IPv6_Packet::IPV6_DEFAULT_HOPLIMIT;
	}

	void IPV6::setHoplimit(uint8_t h)
	{
		hoplimit = h;
	}
	void IPV6::set_dst_addr(const ipv6addr& dst, Interface* interface)
	{
		copy_ipv6_addr(dst, dst_ipv6_addr);
		if (!interface)
			resolveRoute();
		else
			this->interface = interface;
	}

	void IPV6::set_src_addr(const ipv6addr& src)
	{
		copy_ipv6_addr(src, src_ipv6_addr);
	}

	bool IPV6::hasValidSrcDestAddresses() const
	{
		return (src_ipv6_addr.ipaddrB8[0] != 0) && (dst_ipv6_addr.ipaddrB8[0] != 0);
	}
	
	ipstack::AddressEntry* IPV6::resolveRoute()
	{
// 		if (!ipv6.interface || !entry)
// 			return;
// 		
// 		// if we have an on-link address: do nothing
// 		if (IPV6AddressScope::getIPv6AddressScope(entry->ipv6) == IPV6AddressScope::IPV6_SCOPE_LINKLOCAL) {
// 			ipv6.set_nexthop_ndpcache_entry(0);
// 			return;
// 		}
// 		
// 		// We will now look at that entry, if there is a router connected to it.
// 		if (entry->routerEntryPosition == AddressMemory::EntryUndefined) {
// 			// OK, bad situation: The destination address is neither link-local nor a router is known for it.
// 			// This is not our business: Just do nothing
// 			return;
// 		}
// 
// 		// Yes there is a router connected to this entry. Use the routers ip as next hop.
// 		NDPCacheEntry* ndpentry = (NDPCacheEntry*)ipv6.interface->addressmemory.getEntryAtPosition(entry->routerEntryPosition);
// 		if (ndpentry->id != NDPCacheEntry::ID) {
// 			// This should not happen. The entry at the given position is not an ndp entry
// 			// Fix it!
// 			entry->routerEntryPosition = AddressMemory::EntryUndefined;
// 			return;
// 		}
// 
// 		// Use the router as next hop
// 		ipv6.set_nexthop_ndpcache_entry(ndpentry);
// 		
		// No valid destination address
		if (!dst_ipv6_addr.ipaddrB8[0]) {
			interface = 0;
			return 0;
		}
		RouteResult route = find_route(dst_ipv6_addr);
		if (route.proposed_src_addr) {
			copy_ipv6_addr(route.proposed_src_addr->ipv6, src_ipv6_addr);
			if (route.proposed_src_addr->hoplimit) { // if hoplimit is known
				hoplimit = route.proposed_src_addr->hoplimit;
			} else {
				hoplimit = IPv6_Packet::IPV6_DEFAULT_HOPLIMIT;
			}
		} else // Probably a multicast destination address. Just use the unspecified ip as src
			get_unspecified_ipv6_address(src_ipv6_addr);
		interface = route.interface;
		return route.proposed_src_addr;
	}

	bool IPV6::find_route_is_matching(const ipv6addr& ipv6_dstaddr, AddressEntry* entry) {
		return compare_ipv6_addr(ipv6_dstaddr, entry->ipv6, entry->prefixlen);
	}

	RouteResult IPV6::find_route(const ipv6addr& ipv6_dstaddr) {
		Router& router = Router::Inst();
		/**
		 * We will remember an entry for a default router
		 */
		RouteResult backup_default_router;
		
		// Check each interface and all assigned IPv6 prefixes
		Interface* interface = router.head_interface;
		while (interface != 0) {
			// Only check enabled interfaces
			if (interface->isIPv6Up()) {
				uint8_t nextEntry = 0;
				// Check all assigned IPv6 prefixes for the current interface
				while (AddressEntry* entry = interface->addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
					if (find_route_is_matching(ipv6_dstaddr, entry)) {
						// A route has been found (prefix is matching). We do not check for the
						// shortest prefix here. We'll get an icmpv6 redirect message if the target knows a better route
						// anyway.
						return RouteResult(entry, interface);
					}
					// If there is no default route interface assigned yet, and the 
					if (entry->isRouting && !backup_default_router.interface &&
						IPV6AddressScope::getIPv6AddressScope(entry->ipv6) == IPV6AddressScope::IPV6_SCOPE_GLOBAL_UNICAST)
						backup_default_router.proposed_src_addr = entry;
				}
				// Ok, we did not find a valid prefix entry. But may be there are default routers
				if (!backup_default_router.interface) {
					uint8_t nextEntry = 0;
					while (NDPCacheEntry* entry = interface->addressmemory.findEntry<NDPCacheEntryType>(&nextEntry)) {
						if (entry->isRouting) {
							backup_default_router.interface = interface;
							break;
						}
					}
				}
			}
			interface = interface->getNext();
		}
		
		backup_default_router.isDefaultRouter = true;
		return backup_default_router;
	}
}

