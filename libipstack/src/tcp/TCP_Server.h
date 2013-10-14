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

namespace ipstack
{
	/**
	 * This TCP Server implementation consists of two things:
	 * 
	 * Overriden accept() method
	 * -------------------------
	 * If a new incoming connection has been accepted, instead of changing the
	 * tcp server sockets state to established we continue to stay in the listen state
	 * and return one of the tcp sockets out of the pool of sockets.
	 * 
	 * A pool of TCP Sockets
	 * ---------------------
	 * This is useful for a webserver for example, even if you have memory for just 1 socket available,
	 * because after one request from the browser a socket is usually closed. Due to the
	 * necessary delay in the waitstate for two times the maximum segment lifetime (MSL)
	 * a socket's memory cannot be freed/reused. Although the socket in most cases does not need any
	 * memory anymore, because it usually just waits for a timeout, it nevertheless blocks
	 * the memory. If you use a tcp socket pool instead, one SocketMemory
	 * is shared among all tcp sockets returned by the pool.
	 */
	template<unsigned SOCKETS_POOL_SIZE>
	class TCP_Server
	{
		public:
			// Construct with socket memory. This memory is shared accross all sockets including the
			// server socket.
			TCP_Server(SocketMemory* pool_memory, unsigned listenPort);
			~TCP_Server();
			/**
			 * This will put a socket of the pool into listen state and block until a connection
			 * is established. Returns the socket in established state. You have to call accept()
			 * to start listing (with another socket of the pool) again.
			 * @return Return a tcp socket in the established state from the pool of sockets or 0
			 * if either the pool is already used completly or a problem occured.
			 * 
			 * Usage example:
			 * TCP_Socket* clientSocket;
			 * while (clientSocket = server->accept()) {
			 *  // use the client socket
			 * 	clientSocket->write("test",4);
			 *  // free socket and make it usable by the pool again
			 * 	clientSocket->close();
			 * }
			 */
			TCP_Socket* accept();
			
			/// Get size of pool and socket by slot number
			inline unsigned getPoolSize() { return SOCKETS_POOL_SIZE; }
			inline TCP_Socket* getSocketBySlot(unsigned slot) {return (TCP_Socket*)pool[slot*sizeof(TCP_Socket)];}
		private:
			TCP_Server(const TCP_Socket& copy); //prevent copying
			// Reserve memory for tcp sockets
			char pool[SOCKETS_POOL_SIZE*sizeof(TCP_Socket)];
			SocketMemory* sharedmemory;
			unsigned listenPort;
	};
} //namespace ipstack

