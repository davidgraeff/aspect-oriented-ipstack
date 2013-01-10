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

#include "ipstack/router/Interface.h"
#include "ipstack/ipv6/IPv6AddressUtilities.h"
#include "ipstack/ipv6/AddressMemory.h"
#include "ipstack/ipv6/IPv6onSockets.h"
#include "../../Clock.h"
#include "ipstack/SendBuffer.h"
#include "../ICMPv6.h"
#include "../ICMPv6_Socket.h"
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
	
	// Neighbor Solicitation message
	struct NeighborSolicitationMessage {
		UInt8 type; // 135
		UInt8 code; // 0
		UInt16 checksum;
		UInt32 reserved;
		
		ipv6addr target_address;
		UInt8 options[]; // Source link-layer address
	} __attribute__((packed));
	
	// Neighbor Advertisement message
	struct NeighborAdvertisementMessage {
		UInt8 type; // 136
		UInt8 code; // 0
		UInt16 checksum;
		
		UInt8 flags; // |R|S|O|  Reserved | //R: Router; S: Response to NeighborSolicitationMessage; O: Override cache entry
		UInt8 reserved1;
		UInt16 reserved2;
		
		ipv6addr target_address;
		UInt8 options[]; // Source link-layer address
		
		inline bool isRouter() {return 0x80 & flags;}
		inline bool isResponse() {return 0x40 & flags;}
		inline bool isOverride() {return 0x20 & flags;}
		inline void setRouter(bool v) {if (v) flags |= 0x80;}
		inline void setResponse(bool v) {if (v) flags |= 0x40;}
		inline void setOverride(bool v) {if (v) flags |= 0x20;}
	} __attribute__((packed));
	
	// Redirect message
	struct RedirectMessage {
		UInt8 type; // 3
		UInt8 code; // 4
		UInt16 checksum;
		UInt32 reserved;
		
		ipv6addr target_address;
		ipv6addr destination_address;
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
	
	/**
	  * Return the given size modified to be a multiple of 8
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
	
	/**
	  * Check for a duplicate address. If the interface has assigned a duplicate, we have to change the address
	  * and redo the procedure. This implementation only changes one byte: If all 255 available addresses are
	  * duplicates the interface will be unusable. If a device with the same logic and the same start address is within the local network
	  * both devices will never get a usuable interface.
	  */
	static bool is_duplicate_address(const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface) {
		// duplicate address detection
		UInt8 nextEntry = 0;
		while (AddressEntry* addressentry = interface->addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
			if (/*addressentry->state == AddressEntry::AddressEntryStateTemporary &&*/ // Would be nice, but we have no timer -> all addresses are valid until proofed wrong currently
				compare_ipv6_addr(ipv6_dstaddr, addressentry->ipv6)) {
				// Oh no! We have created a duplicate address. "Remove" it from our interface.
// 				printf("is_duplicate_address true!\n");
				interface->addressmemory.freeEntry(addressentry);
			// And add it with a changed byte: Hopefully this will not be a duplicate. We will check that of couse.
				++addressentry->ipv6.ipaddrB8[15];
				interface->registerIPv6Address(addressentry);
				// This will cause a duplicate address detection
				interface->setIPv6AddressState(addressentry, AddressEntry::AddressEntryStateTemporary);
				return true;
				}
		}
		return false;
	}
	
	/**
	  * Prepare a neighbor_solicitation message and send it. You have to provide the src and dest ipv6 addresses
	  * and if the destination address of the actual ipv6 packet should be the solicitedNode multicast address of
	  * the destination address.
	  */
	static void send_neighbor_solicitation(const ipstack::ipv6addr& ipv6_srcaddr, const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface, bool useSolicitatedMulticastAddress) {
		ICMPv6_Socket &icmpv6instance = ICMPv6_Socket::instance();
		IPV6& ipv6 = icmpv6instance.ipv6;
		ipv6.set_dst_addr(ipv6_dstaddr, interface);
		if (useSolicitatedMulticastAddress)
			solicitedNode_multicast_addr(ipv6_dstaddr, (ipv6addr&)ipv6.get_dst_addr());
		ipv6.set_src_addr(ipv6_srcaddr);
		
		const UInt8 resSize = sizeof(NDPMessages::NeighborSolicitationMessage)+multiple_of_octets(interface->getAddressSize())*8;

		SendBufferWithInterface* sbi = icmpv6instance.requestSendBufferICMP(interface, resSize);
		if (sbi) {
			NeighborSolicitationMessage* msg = (NeighborSolicitationMessage*)sbi->sendbuffer.data;
			msg->type = (135);
			msg->code = (0);
			msg->reserved = 0;
			copy_ipv6_addr(ipv6_dstaddr, msg->target_address);
			NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
			sbi->sendbuffer.writtenToDataPointer(resSize);

			icmpv6instance.send(&sbi->sendbuffer, interface);
		}
	}
	
	/**
	  * Prepare a neighbor advertisement message and send it. You have to provide the src and dest ipv6 addresses.
	  * Set @isResponse to true if this advertisement is a response to a neighbor_solicitation message.
	  * Set @isRouter to true if this device is a router.
	  * Set @isOverride always to true except if the destination address is an anycast address.
	  */
	static void send_neighbor_advertisement(const ipstack::ipv6addr& ipv6_srcaddr, const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface,
											bool isResponse, bool isRouter, bool isOverride = true) {
		ICMPv6_Socket &icmpv6instance = ICMPv6_Socket::instance();
		IPV6& ipv6 = icmpv6instance.ipv6;
		ipv6.set_dst_addr(ipv6_dstaddr, interface);
		ipv6.set_src_addr(ipv6_srcaddr);
		
		const UInt8 resSize = sizeof(NDPMessages::NeighborAdvertisementMessage)+multiple_of_octets(interface->getAddressSize())*8;
		
		SendBufferWithInterface* sbi = icmpv6instance.requestSendBufferICMP(interface, resSize);
		if (sbi) {
			NeighborAdvertisementMessage* msg = (NeighborAdvertisementMessage*)sbi->sendbuffer.data;
			msg->type = (136);
			msg->code = (0);
			msg->reserved1 = 0;
			msg->reserved2 = 0;
			msg->flags = 0;
			msg->setRouter(isRouter);
			msg->setResponse(isResponse);
			msg->setOverride(isOverride);
			copy_ipv6_addr(ipv6_srcaddr, msg->target_address);

			NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
			sbi->sendbuffer.writtenToDataPointer(resSize);
			icmpv6instance.send(&sbi->sendbuffer, interface);
		}
	}
};

} //namespace ipstack

