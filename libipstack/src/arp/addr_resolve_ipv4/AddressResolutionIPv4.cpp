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
// Copyright (C) 2011 Christoph Borchert


#include "AddressResolutionIPv4.h"
#include "ip/neighbour_cache/NeighbourCacheConfig.h"
#include "arp/ARPConfig.h"

namespace ipstack {
	const uint8_t* AddressResolutionIPv4::lookup(uint32_t ipv4_addr, Interface* interface){
		uint8_t* hw_addr = find(ipv4_addr);
		if (hw_addr) return hw_addr;
		
		ARP_Socket* socket = Management_Task::Inst().get_socket_arp();
		
		for(uint8_t retries=0; retries < ARP_REQUEST_ATTEMPTS; ++retries){
			// Request
			ipv4_request(ipv4_addr, interface);
			// Wait for arp packet
			uint64_t waitTicks = Clock::ms_to_ticks(NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS);
			uint64_t timeout = Clock::now() + waitTicks;
			while(Clock::now() < timeout) {
				socket->block(waitTicks);
				hw_addr = find(ipv4_addr);
				if (hw_addr) return hw_addr;
			}
		}
		return hw_addr;
	}
	
	// handle incoming arp request: return an arp reply packet to the requesting host 
	void AddressResolutionIPv4::reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, Interface* interface){
		ARP_Socket* socket = Management_Task::Inst().get_socket_arp();
		//TODO
		Eth_ARP_IPv4_Packet* packet = getNextFreePacket();
		if(packet != 0){
		packet->interface = interface;
		
		//Ethernet type field
		packet->get_Eth_Frame()->set_type(ARP_Packet::ETH_TYPE_ARP);

		//ARP Header
		packet->get_ARP_Packet()->set_hw_type(ARP_Packet::ARP_HW_TYPE_ETH);
		packet->get_ARP_Packet()->set_protocol_type(ARP_Packet::ARP_PROTOCOL_TYPE_IPV4);
		packet->get_ARP_Packet()->set_hw_addr_size(ARP_Packet::ARP_HW_ADDR_SIZE_ETH);
		packet->get_ARP_Packet()->set_protocol_addr_size(ARP_Packet::ARP_PROTOCOL_ADDR_SIZE_IPV4);
		packet->get_ARP_Packet()->set_opcode(ARP_Packet::ARP_REPLY);
		
		//Source HW-Addr for this ARP reply packet:
		const uint8_t* i_hw_addr = interface->getAddress();
		memcpy(packet->arp_src_hwaddr, i_hw_addr, 6);
		packet->get_Eth_Frame()->set_src_hwaddr(i_hw_addr);
		
		//Source IP-Addr
		packet->arp_src_ipv4_addr = interface->getIPv4Addr();
		
		//Destination HW-Addr
		memcpy(packet->arp_dst_hwaddr, src_hwaddr, 6);
		packet->get_Eth_Frame()->set_dst_hwaddr(src_hwaddr);
		
		//Destination IP-Addr
		packet->arp_dst_ipv4_addr = *src_ipv4_addr;
		
		//Send the ARP reply packet
		interface->send(packet, Eth_ARP_IPv4_Packet::FRAMESIZE);
		}
	}

	//Send a new ARP-Request packet
	void AddressResolutionIPv4::request(uint32_t ipv4_addr, Interface* interface){
		Eth_ARP_IPv4_Packet* packet = getNextFreePacket();
		if(packet != 0){
		packet->interface = interface;

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
		
		//Send the ARP reply packet
		interface->send(packet, Eth_ARP_IPv4_Packet::FRAMESIZE);
		}
	}
} // namespace ipstack
