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

#include "../cfAttribs.h"
#include "../util/Mempool.h"
#include "ipstack/util/Ringbuffer.h"
#include "ipstack/SendBuffer.h"
#include <string.h>
#include "util/types.h"
#include "stdio.h"
namespace ipstack
{

/**
 * Inherit this to use the management memory. It is important to call the constructor of this super class.
 * All functionality that inherit this management memory class share the same memory blocks and ringbuffer
 */
class ManagementMemory
{
	public:
		/**
		 * Initialise the packetbuffer and mempool pointers to point to the shared memory.
		 * You really want to call this super class constructor in your class!
		 */
		ManagementMemory();
		/**
		 * Construct a SendBuffer (if memory is available) and also remember the destination interface.
		 * This allows us to free unused SendBuffers. A SendBuffer is unused if the linked Interface
		 * has send the data already.
		 */
		SendBufferWithInterface* allocSendBufferWithInterface(UInt16Opt size, Interface* interface);
	private:
		/**
		 * Check all allocated SendBuffers (listed in "packetbuffer") if the data has been send already.
		 * If this is the case, free the memory block / Sendbuffer.
		 * This method is called internally by every call to @allocSendBufferWithInterface.
		 */
		void freeAllUnused();

	protected:
		Packetbuffer* packetbuffer;
		Mempool* mempool;
};


} //namespace ipstack
