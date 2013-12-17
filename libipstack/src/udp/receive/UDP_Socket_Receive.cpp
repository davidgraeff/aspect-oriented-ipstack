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

#include "udp/UDP_Packet.h"
#include "udp/UDP_Socket.h"
#include "demux/Demux.h"
#include <string.h> //for memcpy
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

/**
 * Purspose: add bind/unbind to the socket.
 */

namespace ipstack {
	static uint16_t udp_last_default_port = 1024U;

	bool UDP_Socket::bind() {
		uint16_t sport = get_sport();
		UDP_Socket* current = UDP_Socket::getHead();
		
		if (sport == UDP_Packet::UNUSED_PORT) {
			//choose 'random' source port number
			while (udp_last_default_port < 65000) {
				bool foundUnusedPort = true;
				while (current != 0) {
					if (current->get_sport() == udp_last_default_port) {
						++udp_last_default_port;
						foundUnusedPort = false;
						break;
					}
					current = current->getNext();
				}
				if (foundUnusedPort) {
					set_sport(udp_last_default_port);
					++udp_last_default_port; // increase for next bind()
					break;
				}
			}
			// no free port in range 1024..65000 found!
			return false;
		} else {
			//verify if sport is not used already
			while (current != 0) {
				if (current->get_sport() == sport) {
					//error, sport already in use
					return false;
				}
				current = current->getNext();
			}
		}
		//insert at front
		setNext(UDP_Socket::getHead());
		UDP_Socket::setHead(this);
		return true;
	}

	void UDP_Socket::unbind() {
		if (this == UDP_Socket::getHead()) {
			UDP_Socket::setHead(getNext());
		} else {
			UDP_Socket* current = UDP_Socket::getHead();
			UDP_Socket* next = current->getNext();
			while (next != 0) {
				if (next == thid) {
					current->setNext(next->getNext());
					return;
				}
				current = next;
				next = current->getNext();
			}
		}
		udp_last_default_port = 1024U;
	}
}