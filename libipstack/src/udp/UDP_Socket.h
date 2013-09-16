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
#include "util/ipstack_inttypes.h"
#include "udp/UDP_Socket_Private.h"

namespace ipstack
{

class UDP_Packet;
/**
 * UDP Socket. Depending on your feature selection some of the methods may not be available. You will
 * find more information in the description of each method.
 */
class UDP_Socket : public UDP_Socket_Private, public DemuxLinkedList<UDP_Socket>, public SocketMemory
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
		* If you first would have to concatanate your data, look at the SendBuffer API that is also
		* available on this socket.
		* 
		* @return Returns true if the packet could be delivered to the network hardware. If no
		* free memory is available or len is to big for allocation false is returned.
		* 
		* This method is not implemented, if you have disabled udp send support.
		* 
		* Example usage:
		* udp_socket->send("test", 4);
		*/
		bool send(char* data, int len);
	
		/**
		* The application has to free the memory of the buffer by calling ReceiveBuffer::free(buffer).
		* For more information on ReceiveBuffers look into ReceiveBuffer.h.
		* 
		* @return This method will return 0 if no received data is available and will never block.
		* 
		* This method is not implemented, if you have disabled udp receive support.
		* 
		* Example usage:
		* ReceiveBuffer* b = socket->receive();
		* prinft(b->getData());
		* socket->free(b);
		*/
		ReceiveBuffer* receive();
		
		/**
		* Convenience method: Block until a packet is available or a timeout is reached.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled udp receive support.
		*/
		ReceiveBuffer* receive(uint64_t waitForPacketTimeoutMS);
		
		/**
		* Convenience method: Block until a packet is available.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled udp receive support.
		*/
		ReceiveBuffer* receiveBlock();
		
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
};

} //namespace ipstack
