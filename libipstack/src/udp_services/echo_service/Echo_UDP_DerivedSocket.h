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
#include "udp/UDP_Socket.h"
#include "demux/SystemReceiveCallback.h"
#include "demux/receivebuffer/SmartReceiveBufferPtr.h"

namespace ipstack
{
	/**
	 * We do need to call this socket _DerivedSocket otherwise aspects for sending/receiving
	 * would try to extend this class.
	 */
	class Echo_UDP_DerivedSocket: public UDP_Socket, public SystemReceiveCallback {
	private:
		// no copies
		Echo_UDP_DerivedSocket(const Echo_UDP_DerivedSocket& sp) {}
	public:
		Echo_UDP_DerivedSocket(const SocketMemory& memory) : UDP_Socket(memory) {
			// UDP Echo is on port 7
			set_sport(7);
			bind();
		}
		
		void receiveCallback() {
			while(SmartReceiveBufferPtr b = receive()) {
				uint16_t maxlen = getMaxPayloadLength(b->get_interface());
				if (b->get_payload_size() < maxlen) {
					maxlen = b->get_payload_size();
				}
				send(b->get_payload_data(), maxlen, b->receivebuffer_pointer());
			}
		}
	};
}