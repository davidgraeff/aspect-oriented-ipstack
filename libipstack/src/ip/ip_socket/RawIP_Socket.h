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
#include "demux/receivebuffer/SmartReceiveBufferPtr.h"

namespace ipstack
{

/**
 * Raw IP Socket. Depending on your feature selection some of the methods may not be available. You will
 * find more information in the description of each method. 
 * This type of socket can be used if you want to send raw IP data, like ICMP or want to add
 * your own transport header, for instance for a tcp half-open-request response.
 */
class RawIP_Socket : public SocketMemory
{
	public:
		// Construct with socket memory
		RawIP_Socket(const SocketMemory& memory);
		~RawIP_Socket();
		
		/**
		* Internally creates a sendbuffer with len bytes and copies all content of data to that one.
		* If you first would have to concatanate your data, look at the SendBuffer API that is also
		* available on this socket.
		* @see SendBuffer()
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
		* ip_socket->send("test", 4);
		*/
		bool send(char* data, int len, ReceiveBuffer* use_as_response = 0);
	
		/**
		* Return a smart pointer to a ReceiveBuffer. You can access the payload by using
		* the get_payload_data() method and get the size by get_payload_size().
		* 
		* @return This method will return an invalid smart pointer (=0)
		* if no received data is available and will never block.
		* 
		* This method is not implemented, if you have disabled udp receive support.
		* 
		* Example usage:
		* SmartReceiveBufferPtr b = socket->receive();
		* prinft(b->getData());
		*/
		SmartReceiveBufferPtr receive();
		
		/**
		* Convenience method: Block until a packet is available or a timeout is reached.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled udp receive support.
		*/
		SmartReceiveBufferPtr receive(uint64_t waitForPacketTimeoutMS);
		
		/**
		* Convenience method: Block until a packet is available.
		* This API is only functional if not build with ONE_TASK!
		* This method is not implemented, if you have disabled udp receive support.
		*/
		SmartReceiveBufferPtr receiveBlock();
	protected:
		// block until receive event
		// Not implemented, if receive is disabled
		void block();
};

} //namespace ipstack
