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
// Copyright (C) 2011 Christoph Borchert


#include "TCP_Server.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	TCP_Server::TCP_Server(SocketMemory* pool_memory, unsigned listenPort) :
		sharedmemory(pool_memory), listenPort(listenPort) {
			// set up the memory. All tcp sockets should be in close state
			for (unsigned i=0;i<getPoolSize();++i) {
				// call constructor for the given tcp socket/memory slot
				new(getSocketBySlot(i)) TCP_Socket(*pool_memory);
			}
		}
	
	TCP_Server::~TCP_Server() {
		// abort all socket connections
		for (unsigned i=0;i<getPoolSize();++i) {
			getSocketBySlot(i)->abort();
		}
	}

	TCP_Socket* TCP_Server::accept() {
		TCP_Socket* serverSocket;
		unsigned i=0;
		// Check for a free slot in the pool
		for (;i<getPoolSize();++i) {
			serverSocket = getSocketBySlot(i);
			if (serverSocket->isClosed()) break;
		}
		// No free slot found
		if (i>=getPoolSize()) return 0;
		
		// Setup port and switch to listen state
		serverSocket->set_sport(listenPort);
		if (!serverSocket->listen())
			return 0;
		
		// wait for a connection
		if (serverSocket->accept()) {
			return serverSocket;
		} else
			return 0;
	}
} //namespace ipstack
