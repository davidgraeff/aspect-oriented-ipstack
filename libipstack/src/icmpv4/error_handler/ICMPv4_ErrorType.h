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

#pragma once
#include "ipv4/IPv4_Packet.h"
#include "icmpv4/ICMPv4_Packet.h"
#include "icmpv4/ICMPv4_DerivedSocket.h"
#include "demux/Demux.h"
#include "router/Interface.h"
#include "demux/receivebuffer/ReceiveBuffer.h"

namespace ipstack
{
	class ICMPv4_ErrorReply
	public:
		static void send(ReceiveBuffer* buffer, uint8_t type, uint8_t code) {
			ICMPv4_DerivedSocket& socket = Management_Task::Inst().get_socket_icmpv4();
			
			// As of ICMP RFC we have to echo back part of the errornous packet -> at least the ip header and some data (8 bytes)
			const uint8_t echoErrornousPacketSize = buffer->p.ipv4->get_ihl() * 4 + 8;
			SendBuffer* sbi = socket.requestSendBuffer(buffer->get_interface(),
													echoErrornousPacketSize + ICMPv4_Packet::ICMP_HEADER_SIZE,
													buffer->receivebuffer_pointer());
			if (sbi) {
				sbi->mark("ICMPv4_ErrorReply");
				ICMPv4_Packet* icmp_error_reply = (ICMPv4_Packet*)sbi->getDataPointer();

				icmp_error_reply->set_type(type);
				icmp_error_reply->set_code(code);
				icmp_error_reply->set_quench(0);

				sbi->writtenToDataPointer(ICMPv4_Packet::ICMP_HEADER_SIZE);

				// ICMPv4 RFC: Append at least the errornous ipv4 packet + 8 byte payload to the responding icmp packet
				sbi->write(buffer->p.ipv4, echoErrornousPacketSize);

				icmpv4instance.send(sbi);
			}
		}
	};
}