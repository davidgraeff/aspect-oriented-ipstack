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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "ipstack/ipv6/IPv6AddressUtilities.h"
#include "util/types.h"
#include "ipstack/icmpv6/ndp/NDPMessages.h"
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
		UInt8 type; // 133
		UInt8 code; // 0
		UInt16 checksum;
		UInt32 reserved;
		
		UInt8 options[];
	} __attribute__((packed));
	
	// Router Advertisement message
	struct RouterAdvertisementMessage {
		UInt8 type; // 134
		UInt8 code; // 0
		UInt16 checksum;
		UInt8 curHopLimit; // default value for hop limit; 0:IPV6_DEFAULT
		UInt8 flags; // |M|O|  Reserved | DHCPv6 information available
		UInt16 router_lifetime; // in sec; if 0: no default router
		UInt32 reachable_time;
		UInt32 retrans_time;
		
		UInt8 options[];
	} __attribute__((packed));
	
	// Router advertisement: Prefix Information
	struct OptionPrefixInformation {
		UInt8 type; // 3
		UInt8 len_in_octets; // 4
		UInt8 prefix_length;
		UInt8 flags; // |L|A| Reserved1 | L:on-link; A: stateless address configuration allowed
		UInt32 valid_lifetime; // in sec
		UInt32 preferred_lifetime; // in sec
		UInt8 reserved[4];
		ipv6addr prefix;
		bool isOnLink() {return 0x80 & flags;}
		bool useForStatelessAddressConfiguration() {return 0x40 & flags;}
		UInt32 get_valid_lifetime() { return valid_lifetime; }
		UInt32 get_preferred_lifetime() { return preferred_lifetime; }
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
};

} //namespace ipstack

