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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff

#include "AddressResolutionIPv4.h"
// Arp packets and config
#include "ARP_IPv4_Packet.h"
#include "arp/ARP_Packet.h"
#include "arp/ARP_Socket.h"
#include "arp/ARPConfig.h"
// Neighbour cache
#include "ip/neighbour_cache/NeighbourCacheConfig.h"
#include "ip/neighbour_cache/NeighbourEntry.h"
#include "ip/neighbour_cache/IPv4ToLinkLayer.h"
// System
#include "os_integration/Clock.h"

namespace ipstack {
	const NeighbourEntry* AddressResolutionIPv4::lookup(uint32_t ipv4_addr, Interface* interface){
		volatile NDPCacheEntry* entry = IPv4ToLinkLayer::find(ipv4_addr);
		if (entry && entry->state == NeighbourEntryState_Reachable) return entry;
		
		// Add incomplete 
		IPv4ToLinkLayer::update(ipv4_addr, 0, NeighbourEntry::NeighbourEntryState_Incomplete);
		entry = IPv4ToLinkLayer::find(ipv4_addr);
		if (!entry)
			return 0;
		
		ARP_Socket& socket = Management_Task::Inst().get_socket_arp();
		
		for(uint8_t retries=0; retries < ARP_REQUEST_ATTEMPTS; ++retries){
			// Request
			request(ipv4_addr, interface);
			
			// Wait for arp packet
			uint64_t timeout = Clock::now() + Clock::ms_to_ticks(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
			while(Clock::now() < timeout) {
				socket.block(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
				if (entry->state == NeighbourEntryState_Reachable) return entry;
			}
		}
		return 0;
	}
	
	//Send a new ARP-Request packet
	void AddressResolutionIPv4::request(uint32_t ipv4_addr, Interface* interface) {
		ARP_Socket& socket = Management_Task::Inst().get_socket_arp();
		
		SendBuffer* sendbuffer = SendBuffer::requestRawBuffer(socket, interface, Eth_ARP_IPv4_Packet::FRAMESIZE);
		if (!sendbuffer) return 0;
		
		Eth_ARP_IPv4_Packet* packet = (Eth_ARP_IPv4_Packet*)sbi->getDataPointer();

		//Ethernet type field
		packet->get_Eth_Frame()->set_type(ARP_Packet::ETH_TYPE_ARP);

		//ARP Header
		packet->get_ARP_Packet()->set_hw_type(ARP_Packet::ARP_HW_TYPE_ETH);
		packet->get_ARP_Packet()->set_protocol_type(ARP_Packet::ARP_PROTOCOL_TYPE_IPV4);
		packet->get_ARP_Packet()->set_hw_addr_size(ARP_Packet::ARP_HW_ADDR_SIZE_ETH);
		packet->get_ARP_Packet()->set_protocol_addr_size(ARP_Packet::ARP_PROTOCOL_ADDR_SIZE_IPV4);
		packet->get_ARP_Packet()->set_opcode(ARP_Packet::ARP_REQUEST);
		
		//Source HW-Addr for this ARP reply packet:
		const uint8_t* i_hw_addr = interface->getAddress();
		memcpy(packet->arp_src_hwaddr, i_hw_addr, 6);
		packet->get_Eth_Frame()->set_src_hwaddr(i_hw_addr);
		
		//Source IP-Addr
		packet->arp_src_ipv4_addr = interface->getIPv4Addr();
		
		//Destination HW-Addr
		memset(packet->arp_dst_hwaddr, 0xFF, 6); //Broadcast ethernet address
		packet->get_Eth_Frame()->set_dst_hwaddr_broadcast();
		
		//Destination IP-Addr
		packet->arp_dst_ipv4_addr = ipv4_addr;
		
		//Send the ARP packet
		interface->send(sendbuffer->getDataStart(), sendbuffer->getSize());
		socket.freeSendbuffer(sendbuffer);
	}
} // namespace ipstack
