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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "ipstack/util/Mempool.h"
#include "ipstack/util/Ringbuffer.h"
#include "../demux/DemuxLinkedListContainer.h"
#include <string.h>

namespace ipstack
{

class UDP_Packet;
/**
 * Do not use this UDP_Socket directly but use the API version in IP:UDP_Socket.
 * This class allows to set the ports on a udp socket. Aspect slices are inserted
 * from IPv4 and IPv6 to provide the send and receive functionality.
 */
class UDP_Socket : public DemuxLinkedList<UDP_Socket>
{
	public:
		~UDP_Socket() {}
		
		/// Destination port
		void set_dport(UInt16 d) {
			dport = d;
		}
		UInt16 get_dport() {
			return dport;
		}

		/// Source port
		void set_sport(UInt16 s) {
			sport = s;
		}
		UInt16 get_sport() {
			return sport;
		}

		bool bind() {return false;}
		void unbind() {}
	private:
		Packetbuffer* m_receivequeue;
		Mempool* mempool; // set by API
		UInt16 dport;
		UInt16 sport;

		void setupHeader(UDP_Packet* packet, unsigned datasize) ;

	protected:
		//do not allow construction: only derived classes (API) may be instantiated
		UDP_Socket() ;

		void set_Mempool(Mempool* m) ;
		Mempool* get_Mempool() ;

		void set_ReceiveQueue(Packetbuffer* buf) ;

};

} //namespace ipstack
