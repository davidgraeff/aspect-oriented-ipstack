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


#include "ARP_Cache.h"

namespace ipstack {

	ARP_Cache ARP_Cache::inst_; //create singleton instance

	Eth_ARP_IPv4_Packet* ARP_Cache::getNextFreePacket(){
		for(unsigned i=0; i<MAX_PACKETS; i++){
			Eth_ARP_IPv4_Packet& packet = packet_pool[i];
			if(packet.isFree()){
			return &packet;
			}
		}
		return 0;
	}


	uint8_t* ARP_Cache::ipv4_find(uint32_t ipv4_addr){
		for(unsigned i=0; i<MAX_CACHE_ENTRIES; i++){      
			cache_entry& entry = entries[i];
			if(entry.ipv4_addr == ipv4_addr){
			return entry.hw_addr;
			}
		}
		return 0;
	}

	void ARP_Cache::ipv4_wait(uint32_t ipv4_addr){
		uint64_t timeout = Clock::now() + Clock::ms_to_ticks(1000U); //1 sec
		while( (Clock::now() < timeout) && (ipv4_find(ipv4_addr)==0) );
	}
	
	void ARP_Cache::clear() {
		for(unsigned i=0; i<MAX_CACHE_ENTRIES; i++){      
			entries[i].ipv4_addr = 0;
		}
	}


	const uint8_t* ARP_Cache::ipv4_lookup(uint32_t ipv4_addr, Interface* interface){
		uint8_t* hw_addr = ipv4_find(ipv4_addr);
		for(int retries=0; (hw_addr==0) && (retries<3); retries++){
			ipv4_request(ipv4_addr, interface);
			ipv4_wait(ipv4_addr);
			hw_addr = ipv4_find(ipv4_addr);
		}
		return hw_addr;
	}
	
	// handle incoming arp request: return an arp reply packet to the requesting host 
	void ARP_Cache::ipv4_reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, Interface* interface){
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
	
	// handle incoming arp replies: update own cache
	void ARP_Cache::ipv4_update(const uint32_t* ipv4_addr, const uint8_t* hw_addr){
		unsigned selected_index = ARP_Cache::MAX_CACHE_ENTRIES;
		unsigned first_free_index = ARP_Cache::MAX_CACHE_ENTRIES;
		
		for(unsigned i=0; i<ARP_Cache::MAX_CACHE_ENTRIES; i++){      
		ARP_Cache::cache_entry& entry = entries[i];
		if(entry.ipv4_addr == *ipv4_addr){
			selected_index = i;
			break;
		}
		else if(entry.ipv4_addr == 0){
			if(first_free_index == ARP_Cache::MAX_CACHE_ENTRIES){
			first_free_index = i;
			}
		}
		}
		if(selected_index == ARP_Cache::MAX_CACHE_ENTRIES){
		//ipv4_addr is not in cache table yet
		if(first_free_index != ARP_Cache::MAX_CACHE_ENTRIES){
			//there is a free cache slot left
			selected_index = first_free_index;
		}
		else{
			//evict cache entry
			//TODO
			selected_index = 0; //implement strategy
		}
		}

		ARP_Cache::cache_entry& entry = entries[selected_index];
		entry.ipv4_addr = *ipv4_addr;
		memcpy(entry.hw_addr, hw_addr, 6);
	}
	

	//Send a new ARP-Request packet
	void ARP_Cache::ipv4_request(uint32_t ipv4_addr, Interface* interface){
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
