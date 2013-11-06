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

#pragma once

#include "util/ipstack_inttypes.h"
#include "arp/ARP_Packet.h"

namespace ipstack {
	class Eth_ARP_IPv4_Packet {
	public:
		ARP_Packet arp_header;
		//ARP Payload for IPv4
		//Source (sender):
		uint8_t arp_src_hwaddr[ARP_Packet::ARP_HW_ADDR_SIZE_ETH];
		uint32_t arp_src_ipv4_addr;
		//Destination (receiver):
		uint8_t arp_dst_hwaddr[ARP_Packet::ARP_HW_ADDR_SIZE_ETH];
		uint32_t arp_dst_ipv4_addr;
	} __attribute__ ((packed));
} // namespace ipstack
