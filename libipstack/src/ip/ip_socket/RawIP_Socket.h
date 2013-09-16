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

#include "memory_management/SocketMemory.h"
#include "util/ipstack_inttypes.h"
#include "ip/ip_socket/RawIP_Socket_Private.h"

namespace ipstack
{

/**
 * Raw IP Socket. Depending on your feature selection some of the methods may not be available. You will
 * find more information in the description of each method. 
 * This type of socket can be used if you want to send raw IP data, like ICMP or want to add
 * your own transport header, for instance for a tcp half-open-request response.
 */
class RawIP_Socket : public RawIP_Socket_Private, public SocketMemory
{
	public:
		// Construct with socket memory
		RawIP_Socket(const SocketMemory& memory);
		~RawIP_Socket();
		
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
		* ReceiveBuffer::free(b);
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
};

} //namespace ipstack
