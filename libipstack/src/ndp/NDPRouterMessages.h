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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "ipv6/IPv6AddressUtilities.h"
#include "util/ipstack_inttypes.h"
#include "icmpv6/ndp/NDPMessages.h"
namespace ipstack
{
/**
 * Purpose: This class defines some commen functionality and defines
 * all structs for NDP router messages.
 */
class NDPRouterMessages
{
public:
	// Router Solicitation message
	struct RouterSolicitationMessage {
		uint8_t type; // 133
		uint8_t code; // 0
		uint16_t checksum;
		uint32_t reserved;
		
		uint8_t options[];
	} __attribute__((packed));
	
	// Router Advertisement message
	struct RouterAdvertisementMessage {
		uint8_t type; // 134
		uint8_t code; // 0
		uint16_t checksum;
		uint8_t curHopLimit; // default value for hop limit; 0:IPV6_DEFAULT
		uint8_t flags; // |M|O|  Reserved | DHCPv6 information available
		uint16_t router_lifetime; // in sec; if 0: no default router
		uint32_t reachable_time;
		uint32_t retrans_time;
		
		uint8_t options[];
	} __attribute__((packed));
	
	// Router advertisement: Prefix Information
	struct OptionPrefixInformation {
		uint8_t type; // 3
		uint8_t len_in_octets; // 4
		uint8_t prefix_length;
		uint8_t flags; // |L|A| Reserved1 | L:on-link; A: stateless address configuration allowed
		uint32_t valid_lifetime; // in sec
		uint32_t preferred_lifetime; // in sec
		uint8_t reserved[4];
		ipv6addr prefix;
		bool isOnLink() {return 0x80 & flags;}
		bool useForStatelessAddressConfiguration() {return 0x40 & flags;}
		uint32_t get_valid_lifetime() { return valid_lifetime; }
		uint32_t get_preferred_lifetime() { return preferred_lifetime; }
	} __attribute__((packed));

	
	static OptionPrefixInformation* get_prefix_option(char* data, unsigned remaining_options_length) {
		while (1) {
			OptionPrefixInformation* option = (OptionPrefixInformation*)data;
			if (!option->len_in_octets || option->len_in_octets*8>remaining_options_length)
				return 0; // drop packet with option length = 0: http://tools.ietf.org/html/rfc4861#section-4.6
				if (option->type == 3) {
					return option;
				}
				// we do not parse type 4: Redirected Header here: This is just the redirected packet. We drop it for simplicity.
				remaining_options_length -= option->len_in_octets * 8;
			data += option->len_in_octets * 8;
		}
		return 0;
	}
	
	static void send_router_solicitation(ipstack::ipv6addr ipv6_dstaddr, Interface* interface);
	static void update_router_entry(Interface* interface);
};

} //namespace ipstack

