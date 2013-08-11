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

#include "NDPNeighborMessages.h"
#include "ipstack/demux/Demux.h"
namespace ipstack
{


/**
	* Prepare a neighbor_solicitation message and send it. You have to provide the src and dest ipv6 addresses
	* and if the destination address of the actual ipv6 packet should be the solicitedNode multicast address of
	* the destination address.
	*/
void NDPNeighborMessages::send_neighbor_solicitation(const ipstack::ipv6addr& ipv6_srcaddr, const ipstack::ipv6addr& ipv6_dstaddr, Interface* interface, bool useSolicitatedMulticastAddress) {
	ICMPv6_Socket &icmpv6instance = ICMPv6_Socket::instance();
	IPV6& ipv6 = icmpv6instance.ipv6;
	ipv6.set_dst_addr(ipv6_dstaddr, interface);
	if (useSolicitatedMulticastAddress)
		solicitedNode_multicast_addr(ipv6_dstaddr, (ipv6addr&)ipv6.get_dst_addr());
	ipv6.set_src_addr(ipv6_srcaddr);
	
	const UInt8 resSize = sizeof(NeighborSolicitationMessage)+NDPMessages::multiple_of_octets(interface->getAddressSize())*8;

	Demux::Inst().setDirectResponse(true);
	SendBuffer* sbi = icmpv6instance.requestSendBuffer(interface, resSize);
	if (sbi) {
		sbi->mark("neighbor_solicitation");
		NeighborSolicitationMessage* msg = (NeighborSolicitationMessage*)sbi->getDataPointer();
		msg->type = (135);
		msg->code = (0);
		msg->reserved = 0;
		copy_ipv6_addr(ipv6_dstaddr, msg->target_address);
		NDPMessages::write_option_linklayer_address(NDPMessages::SourceLinkLayer, (char*)msg->options, interface->getAddress(), interface->getAddressSize());
		sbi->writtenToDataPointer(resSize);

		icmpv6instance.send(sbi);
	}
}

} //namespace ipstack

