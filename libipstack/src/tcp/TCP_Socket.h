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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff
#pragma once

#include "memory_management/SocketMemory.h"
#include "demux/DemuxLinkedListContainer.h"
#include "TCP_Socket_Private.h"
#include "router/sendbuffer/sendbufferAPI.h"
#include "util/ipstack_inttypes.h"
#include "ip/IP.h"

namespace ipstack
{
	/**
	 * Public API for a TCP Socket
	 */
	class TCP_Socket : public TCP_Socket_Private, public DemuxLinkedList<TCP_Socket>,
		public SocketMemory, public SendBufferAPI, public IP
	{
		public:
			// **************************************************************************
			// *** Public API ***
			// **************************************************************************
			
			// Construct with socket memory
			TCP_Socket(const SocketMemory& memory);
			~TCP_Socket();

			/**
			 * Change to listen state. An incoming connection can be established now.
			 * This will only work if the tcp socket is not connected so far and a
			 * source port is set up.
			 * @return Return true if socket changed to listen state successfully
			 */
			bool listen();
			
			/**
			 * Connect to the destination port and ip. You do not have to explicitly
			 * set up a source port, a free one will be choosen for you.
			 * This will only work if the tcp socket is not connected so far.
			 * @return Return true if the connection could be established.
			 * 
			 * This method will block.
			 */
			bool connect();

			/**
			 * Use this method to write data over this tcp connection.
			 * Another way is to use the streaming operator. Usage example:
			 * TCP_Socket t;
			 * t << "some data" << "another data" << 11;
			 */
			bool write(const void* data, unsigned datasize);
			
			/**
			 * Use this api to receive data over this tcp connection.
			 * This method will block indefinitely until data is received.
			 * Use poll(msec) before if you need timeout behaviour.
			 * @return The number of received bytes or -1 if no connection
			 * is established so far or -2 if the remote host requests to close
			 * the connection.
			 */
			int receive(void* buffer, unsigned buffer_len);
			
			/**
			 * wait for msec for a new packet to arive. If you do not
			 * specify a waiting time the method will return immediately.
			 */
			int poll(unsigned msec=0);
			
			/**
			 * Full close a tcp connection (half-close/receiving-only not supported)
			 * @return Return true if the socket is in close state.
			 * 
			 * This method will block.
			 */
			bool close();
			
			/// Destination port. Do not change this if connected!
			void set_dport(uint16_t d) ;
			uint16_t get_dport() ;
			
			/// Source port. Do not change this if connected!
			void set_sport(uint16_t s) ;
			uint16_t get_sport() ;
			
			// **************************************************************************
			// *** Advanced Public API ***
			// **************************************************************************
			
			/**
			 * Close a connection by freeing up all resources without requesting
			 * a close from the remote host. The state after this operation is "close".
			 * 
			 * Please be gentle to the remote host and do not use this method!
			 * A remote host may not detect the broken connection for a long period.
			 */
			void abort();

			/**
			 * Window size for receiving
			 * @param max_mss Receive window size, 1 means you can receive 1 tcp segment
			 * and have to fetch the data with receive(...) before another segment can be
			 * received.
			 */
			void setMaxReceiveWindow(unsigned max_mss);
		private:
			TCP_Socket(const TCP_Socket& copy); //prevent copying
	};
} //namespace ipstack

