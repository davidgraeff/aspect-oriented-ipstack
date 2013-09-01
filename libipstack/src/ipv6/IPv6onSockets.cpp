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

	ipstack::AddressEntry* IPV6::resolveRoute()
	{
		// No valid destination address
		if (!dst_ipv6_addr.ipaddrB8[0]) {
			interface = 0;
			return 0;
		}
		Router& router = Router::Inst();
		RouteResult route = router.ipv6_find_route(dst_ipv6_addr);
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
}

