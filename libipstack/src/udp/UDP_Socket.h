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
// Copyright (C) 2011 Christoph Borchert, 2012-2013 David Gräff

#pragma once

#include "memory_management/SocketMemory.h"
#include "demux/DemuxLinkedListContainer.h"
#include "demux/receivebuffer/SmartReceiveBufferPtr.h"
#include "router/sendbuffer/sendbufferAPI.h"
#include "util/ipstack_inttypes.h"
#include "ip/IP.h"

namespace ipstack
{

class UDP_Packet;
/**
 * UDP Socket. Receiving API is made available by the SocketMemory class (receive()/receiveBlock())
 * and DemuxLinkedList class (bind()/unbind()) (if receiving is enabled!).
 * For sending you may either use the convenience API (send(char* data, int len)) or the SendBuffer
 * API directly.
 * 
 * TODO AspectC++2: Use templates for IP version instead of inherit IP
 */
class UDP_Socket: public DemuxLinkedList<UDP_Socket>, public SocketMemory, public SendBufferAPI, public IP
{
	public:
		// Construct with socket memory
		UDP_Socket(const SocketMemory& memory);
		~UDP_Socket();
		
		/// Destination port
		void set_dport(uint16_t d);
		uint16_t get_dport();

		/// Source port
		void set_sport(uint16_t s);
		uint16_t get_sport();

		/**
		* Internally creates a sendbuffer with len bytes and copies all content of data to that one.
		* If you first would have to concatenate your data, look at the SendBuffer API that is also
		* available on this socket and is more efficient for those scenarios.
		* 
		* @param use_as_response If you provide a receiveBuffer as input, the @data is send
		* to the referred source/remote host. Warning: This ignores all set_ip and set_port methods
		* you issued before!
		* @return Returns true if the packet could be delivered to the network hardware. If no
		* free memory is available or len is to big for allocation false is returned.
		* 
		* This method is not implemented, if you have disabled udp send support.
		* 
		* Example usage:
		* udp_socket->send("test", 4);
		*/
		bool send(char* data, int len, ReceiveBuffer* use_as_response = 0);

		/**
		 * Bind this socket to the source port you have set before. Without binding
		 * the socket, receive will not work as expected.
		 * You do not need to call unbind/bind to set a new source port.
		 */
		bool bind();
		/**
		 * Unbind this socket from the source port you have set before.
		 * This is called automatically on destruction of the socket. 
		 * You do not need to call unbind/bind to set a new source port.
		 */
		void unbind();
	protected:
		void setupHeader(UDP_Packet* packet, unsigned datasize) ;
		uint16_t dport;
		uint16_t sport;
};

} //namespace ipstack
