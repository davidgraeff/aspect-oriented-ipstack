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

namespace ipstack
{
	/**
	 * Public API for a TCP Socket
	 */
	class TCP_Socket : public TCP_Socket_Private, public DemuxLinkedList<TCP_Socket>, public SocketMemory
	{
		public:
			// **************************************************************************
			// *** Public API ***
			// **************************************************************************
			
			// Construct with socket memory
			TCP_Socket(const SocketMemory& memory);
			
			/**
			* Bind this socket to the source port you have set before.
			* You do not need to call unbind/bind to set a new source port, but you have to
			* close the TCP connection before chanching the port.
			*/
			bool bind();
			
			/**
			* Unbind this socket from the source port you have set before.
			* This is called automatically on destruction of the socket. 
			* You do not need to call unbind/bind to set a new source port.
			*/
			void unbind();
			
			/**
			* Listen on the source port for incoming traffic.
			*/
			bool listen();
			
			/**
			* Use this method to write data over this tcp connection.
			* Another way is to use the streaming operator. Usage example:
			* TCP_Socket t;
			* t << "some data" << "another data" << 11;
			*/
			bool write(const void* data, unsigned datasize);
			
			/**
			* Use this api to receive data over this tcp connection.
			*/
			int receive(void* buffer, unsigned buffer_len);
			
			/**
			* wait for msec for a new packet to arive. If yyou do not
			* specify a waiting time the method will return immediately.
			*/
			int poll(unsigned msec=0);
			
			/**
			* close a tcp connection
			*/
			bool close();
			
			/// Destination port
			void set_dport(uint16_t d) ;
			uint16_t get_dport() ;
			
			/// Source port
			void set_sport(uint16_t s) ;
			uint16_t get_sport() ;
			
			void setMaxReceiveWindow(unsigned max);

			~TCP_Socket();
		private:
			TCP_Socket(const TCP_Socket& copy); //prevent copying
	};

	/**
	* Streaming operator for the TCP_Socket.
	* Usage example:
	* TCP_Socket t;
	* t << "some data" << "another data" << 11;
	*/
	template<class T> inline TCP_Socket &operator <<(TCP_Socket &obj, T arg) { obj.write(arg,sizeof(arg)); return obj; }
	template<> inline TCP_Socket &operator <<(TCP_Socket &obj, const char* arg) { obj.write(arg,strlen(arg)); return obj; }


} //namespace ipstack

