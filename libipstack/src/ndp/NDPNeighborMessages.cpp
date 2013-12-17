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

#include "NDPNeighborMessages.h"
#include "demux/Demux.h"
#include "ip/SendbufferIP.h"

namespace ipstack {
	/**
	* Prepare a neighbor_solicitation message and send it. You have to provide the src and dest ipv6 addresses
	* and if the destination address of the actual ipv6 packet should be the solicitedNode multicast address of
	* the destination address.
	*/
	void NDPNeighborMessages::send_neighbor_solicitation(const ipstack::ipv6addr& ipv6_srcaddr, const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface, bool useSolicitatedMulticastAddress) {
		ICMPv6_Socket &socket = ICMPv6_Socket::instance();
		IPV6& ipv6 = socket.ipv6;
		ipv6.set_dst_addr(ipv6_dstaddr, interface);
		if (useSolicitatedMulticastAddress)
			solicitedNode_multicast_addr(ipv6_dstaddr, (ipv6addr&)ipv6.get_dst_addr());
		ipv6.set_src_addr(ipv6_srcaddr);
		
		const uint8_t resSize = sizeof(NeighborSolicitationMessage)+NDPMessages::multiple_of_octets(interface->getAddressSize())*8;

		SendBuffer* sbi = SendbufferIP::requestIPBuffer(socket, socket, resSize);
		if (sbi) {
			sbi->mark("neighbor_solicitation");
			NeighborSolicitationMessage* msg = (NeighborSolicitationMessage*)sbi->getDataPointer();
			msg->type = (135);
			msg->code = (0);
			msg->reserved = 0;
			copy_ipv6_addr(ipv6_dstaddr, msg->target_address);
			NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
			sbi->writtenToDataPointer(resSize);
			sbi->send();
		}
	}

	// handle incoming Neighbour Solicitation request by
	// returning a Neighbour Advertisement packet to the requesting host
	void NDPNeighborMessages::reply(const ipv6addr& src_addr, ReceiveBuffer& b) {
		ICMPv6_Socket &socket = ICMPv6_Socket::instance();
		IPV6& ipv6 = socket.ipv6;
		ipv6.set_dst_addr(packet->get_src_ipaddr(), interface);
		ipv6.set_src_addr(src_addr);
		
		const uint8_t resSize = sizeof(NeighborAdvertisementMessage)+NDPMessages::multiple_of_octets(interface->getAddressSize())*8;
		
		demux.setDirectResponse(true);
		SendBuffer* sbi = SendbufferIP::requestIPBuffer(socket, socket, resSize);
		if (sbi) {
			sbi->mark("AddressResolutionResponse");
			NeighborAdvertisementMessage* msg = (NeighborAdvertisementMessage*)sbi->getDataPointer();
			msg->type = (136);
			msg->code = (0);
			msg->reserved1 = 0;
			msg->reserved2 = 0;
			msg->flags = 0;
			msg->setRouter(false);
			msg->setResponse(true);
			msg->setOverride(true);
			copy_ipv6_addr(src_addr, msg->target_address);

			NDPMessages::write_option_linklayer_address(NDPMessages::TargetLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
			sbi->writtenToDataPointer(resSize);
			sbi->send();
		}
	}

	// Send a Neighbour Solicitation packet
	void NDPNeighborMessages::request(const ipv6addr& dest_addr, uint8_t state, Interface* interface) {
		
		// If the entry is invalid or if the link layer address is already known do nothing
		if (state == NDPCacheEntry::NDPCacheEntryState_Reachable)
			return;
		
		ipstack::ipv6addr ipv6_srcaddr = {{0}};
		if (interface->getInterfaceIPv6AddressByScope(ipv6_srcaddr, IPV6AddressScope::IPV6_SCOPE_LINKLOCAL)) {

			ICMPv6_Socket &socket = ICMPv6_Socket::instance();
			IPV6& ipv6 = socket.ipv6;
			// we use the multicast IP if the state is "incomplete". If the state is "stale" the link layer address
			// is already known and just has to be verified.
			if (state != NDPCacheEntry::NDPCacheEntryState_Stale)
				solicitedNode_multicast_addr(dest_addr, (ipv6addr&)ipv6.get_dst_addr());
			else
				ipv6.set_dst_addr(dest_addr, interface);
			
			ipv6.set_src_addr(ipv6_srcaddr);
			
			const uint8_t resSize = sizeof(NeighborSolicitationMessage)+NDPMessages::multiple_of_octets(interface->getAddressSize())*8;

			Demux::Inst().setDirectResponse(true);
			SendBuffer* sbi = SendbufferIP::requestIPBuffer(socket, socket, resSize);
			if (sbi) {
				sbi->mark("AddressResolutionRequest");
				NeighborSolicitationMessage* msg = (NeighborSolicitationMessage*)sbi->getDataPointer();
				msg->type = (135);
				msg->code = (0);
				msg->reserved = 0;
				copy_ipv6_addr(dest_addr, msg->target_address);
				NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
				sbi->writtenToDataPointer(resSize);
				sbi->send();
			}
		}
	}
} //namespace ipstack

