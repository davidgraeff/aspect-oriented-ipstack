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
// Copyright (C) 2013 David Gräff

#include "udp/UDP_Socket.h"
#include "ip/SendbufferIP.h"
#include "util/ipstack_inttypes.h"

namespace ipstack {
	bool UDP_Socket::send(char* data, int len, ReceiveBuffer* use_as_response) {
		SendBuffer* sendbuffer = requestSendbuffer(len, use_as_response);
		if (!sendbuffer)
			return false;
		sendbuffer->write(data, len);
		sendbuffer->send();
		freeSendbuffer(sendbuffer);
		return true;
	}
	
	SendBuffer* UDP_Socket::requestSendbuffer(int len, ReceiveBuffer* use_as_response) {
		SendBuffer* sendbuffer = SendbufferIP::requestIPBuffer(*this, *this, len + UDP_Packet::UDP_HEADER_SIZE, use_as_response);
		if (!sendbuffer)
			return 0;
		
		// set ip next header protocol to UDP
		*(sendbuffer->p.nextheaderfield_ip) = UDP_Packet::IP_TYPE_UDP;

		UDP_Packet* udp_packet = sendbuffer->getDataPointer();
		sendbuffer->p.transport_packet = udp_packet;
		setupHeader(udp_packet, sendbuffer->getRemainingSize());
		sendbuffer->writtenToDataPointer(UDP_Packet::UDP_HEADER_SIZE);

		// Proceed if direct response is off or if this should not be a reponse to a packet
		if (!use_as_response) {
			tjp->proceed();
			return;
		}

		// Set destination addr := remote src addr
		const UDP_Packet* remote_udp_packet = (UDP_Packet*)use_as_response->p.transport_packet;
		udp_packet->set_dport(remote_udp_packet->get_sport());
	}
	
}