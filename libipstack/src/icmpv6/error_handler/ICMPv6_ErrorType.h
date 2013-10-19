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
#include "ipv6/IPv6_Packet.h"
#include "icmpv6/ICMPv6_Packet.h"
#include "icmpv6/ICMPv6_Socket.h"
#include "demux/Demux.h"
#include "router/Interface.h"
#include "demux/receivebuffer/ReceiveBuffer.h"

namespace ipstack
{
	class ICMPv6_ErrorReply
	public:
		struct ICMPv6Unrechable {
			uint8_t type;
			uint8_t code;
			uint16_t checksum;
			uint32_t failure_pointer;
			uint8_t original_packet[];
		} __attribute__((packed));
		
		static void send(ReceiveBuffer& buffer, uint8_t type, uint8_t code, uint32_t failure_pointer) {
			ICMPv6_Socket* socket = Management_Task::Inst().get_socket_icmpv6();
			
			// As of ICMPv6 RFC we have to echo back part of the errornous packet -> at least the ip header and some data (8 bytes)
			SendBuffer* sbi = socket->requestSendBuffer(ICMPv6_Packet::ICMP_MAX_DATA_SIZE,
													buffer.receivebuffer_pointer());
			if (sbi) {
				sbi->mark("ICMPv6_ErrorReply");
				ICMPv6Unrechable* reply = (ICMPv6Unrechable*)sbi->getDataPointer();

				reply->type = type;
				reply->code = code;
				reply->failure_pointer = failure_pointer;				
				sbi->writtenToDataPointer(sizeof(ICMPv6Unrechable));

				// ICMPv6 RFC: Append at least the errornous ipv6 packet + 8 byte payload to the responding icmp packet
				sbi->write(packet, ICMPv6_Packet::ICMP_MAX_DATA_SIZE-sizeof(ICMPv6Unrechable));

				socket->send(sbi);
			}
		}
	};
}