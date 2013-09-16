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
#pragma once
#include <string.h> //for memcpy
#include "util/ipstack_inttypes.h"
#include "ethernet/Eth_Frame.h"
#include "router/Interface.h"
#include "os_integration/Clock.h"

#include "Eth_ARP_IPv4_Packet.h"
#include "ARP_Packet.h"


namespace ipstack {

class ARP_Cache {
	public:
	static ARP_Cache& Inst() { return inst_; } //get singleton

	const uint8_t* ipv4_lookup(uint32_t ipv4_addr, Interface* interface);
	// handle incoming arp request: return an arp reply packet to the requesting host 
	void ipv4_reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, Interface* interface);
	// handle incoming arp replies: update own cache
	void ipv4_update(const uint32_t* ipv4_addr, const uint8_t* hw_addr);
	//Send a new ARP-Request packet
	void ipv4_request(uint32_t ipv4_addr, Interface* interface);
	
	private:
	
	uint8_t* ipv4_find(uint32_t ipv4_addr);
	void ipv4_wait(uint32_t ipv4_addr);
	void clear();

	//singleton design pattern
	static ARP_Cache inst_;
	ARP_Cache() {} //ctor hidden
	ARP_Cache(const ARP_Cache&); //copy ctor hidden
	ARP_Cache& operator=(ARP_Cache const&); // assign op. hidden
	
	enum { MAX_PACKETS = 1 }; // arp packets to be send concurrently
	enum { MAX_CACHE_ENTRIES = 5 };
	struct cache_entry {
		uint32_t ipv4_addr;
		uint8_t hw_addr[6];
	} entries[MAX_CACHE_ENTRIES];
	Eth_ARP_IPv4_Packet packet_pool[MAX_PACKETS]; //the packet pool
	Eth_ARP_IPv4_Packet* getNextFreePacket();
};

} // namespace ipstack
