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
// Copyright (C) 2012 David Gräff

#pragma once

#include "ipstack/ipv6/IPv6AddressUtilities.h"
#include "util/types.h"
namespace ipstack
{
/**
 * Purpose: This class defines some commen functionality and defines
 * all structs for NDP messages.
 */
class NDPMessages
{
public:
	// Source link layer: Neighbor Solicitation, Router Solicitation, and Router Advertisement
	// Target Link Layer: Neighbor Advertisement and Redirect packets
	struct OptionSourceTargetLinkLayerAddress {
		UInt8 type; // 1 for Source Link-layer Address, 2 for Target Link-layer Address
		UInt8 len_in_octets;
		UInt8 link_layer_address[];
	} __attribute__((packed));
	
	// MTU Option
	struct OptionMTU {
		UInt8 type; // 5
		UInt8 len_in_octets; // = 1
		UInt8 mtu[4];
	} __attribute__((packed));
	
	enum LinkLayerAddressType {SourceLinkLayer=1,TargetLinkLayer=2};
	static bool get_option_linklayer_address(LinkLayerAddressType type, char* data, unsigned remaining_options_length, char mac_address[6]) {
		while (1) {
			OptionSourceTargetLinkLayerAddress* option = (OptionSourceTargetLinkLayerAddress*)data;
			if (!option->len_in_octets || option->len_in_octets*8>remaining_options_length)
				return 0; // drop packet with option length = 0: http://tools.ietf.org/html/rfc4861#section-4.6
			if (option->type == type) { // option 2 for Target Link-layer Address
				memcpy(mac_address, option->link_layer_address, 6);
				return true;
			}
			// we do not parse type 4: Redirected Header here: This is just the redirected packet. We drop it for simplicity.
			remaining_options_length -= option->len_in_octets * 8;
			data += option->len_in_octets * 8;
		}
		return false;
	}

	/**
	  * Count how many octets are necessary for origSize.
	  */
	static UInt8 multiple_of_octets(UInt8 origSize) {
		// 1: add one unit per 8 bytes additional address length
		// 2: add one additional unit if origSize is not a multiple of 8
		return origSize / 8 + (origSize % 8)? 1 : 0;
	}

	/**
	  * neighbor_solicitation and other ndp messages may send an additional option: The link layer address (source or target address)
	  */
	static void write_option_linklayer_address(LinkLayerAddressType type, char* data, const unsigned char* linklayerAddress, UInt8 linklayerAddressSize) {
		OptionSourceTargetLinkLayerAddress* option = (OptionSourceTargetLinkLayerAddress*)data;
		option->type = type;
		option->len_in_octets = 1;
		// A "OptionSourceTargetLinkLayerAddress" option is 8 byte in length. type+size are 2 bytes so 6 bytes remain for the link layer address.
		// If the link layer address is longer than 6 bytes we have to adjust "len_in_octets". 
		if (linklayerAddressSize>6) {
			option->len_in_octets += multiple_of_octets(linklayerAddressSize);
		}
		memcpy(option->link_layer_address,linklayerAddress,linklayerAddressSize);
	}
};

} //namespace ipstack

