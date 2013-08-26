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

#include "cfAttribs.h"
#include "../util/Mempool.h"
#include "util/Ringbuffer.h"
#include "SendBuffer.h"
#include "ip_management_memory/ManagementMemory.h"
#include <string.h>
#include "util/types.h"

namespace ipstack
{

/**
 * Allows UDP Management Services like udp echo/reply
 */
class Management_UDP_Socket : public ManagementMemory
{
	public:
		Management_UDP_Socket() : ManagementMemory() {}
		~Management_UDP_Socket() {}

		static Management_UDP_Socket& instance() {return m_instance;}
		
		//for placement new: calling the constructor explicitly for global objects
		void* operator new(__SIZE_TYPE__ size, void* mem) {	return mem;	}
	private:
		Management_UDP_Socket(const Management_UDP_Socket &) {} // no copies and BUG in aspectc++ (segfault if not defined)
		static Management_UDP_Socket m_instance;
};

} //namespace ipstack
