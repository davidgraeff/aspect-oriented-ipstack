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


#include "AddressReplyIPv4.h"
#include "ip/neighbour_cache/NeighbourCacheConfig.h"
// Arp packets and config
#include "ARP_IPv4_Packet.h"
#include "arp/ARP_Packet.h"
#include "arp/ARP_Socket.h"
#include "arp/ARPConfig.h"

namespace ipstack {
	// handle incoming arp request: return an arp reply packet to the requesting host 
	void AddressReplyIPv4::reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, Interface* interface) {
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
		
		//Send the ARP packet
		interface->send(sendbuffer->getDataStart(), sendbuffer->getSize());
		socket.freeSendbuffer(sendbuffer);
		}
	}
} // namespace ipstack
