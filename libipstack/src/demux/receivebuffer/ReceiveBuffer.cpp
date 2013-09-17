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
// Copyright (C) 2012 David Gräff


#include "ReceiveBuffer.h"

namespace ipstack
{
	ReceiveBuffer::ReceiveBuffer(Interface* interface, char* data, uint16_t receivedSize) :
		m_interface(interface), m_data(data), m_receivedSize(receivedSize) {};
	
	ReceiveBuffer* ReceiveBuffer::clone(SocketMemory& socket_mem, void* start_mem = 0) {
		if (cloned)
			return this;
		
		// set start position for copy if not set
		if (!start_mem)
			start_mem = m_data;
		
		const int copy_length = m_receivedSize - (start_mem-m_data);
		ReceiveBuffer* r = (ReceiveBuffer*)socket_mem.get_Mempool()->alloc(copy_length + sizeof(ReceiveBuffer));
		if (!r)
			return 0;
		
		r->cloned = true;
		r->protocol_pointer = protocol_pointer;
		r->m_data = getDataStart();
		r->m_receivedSize = copy_length;
		memcpy(r->getDataStart(), m_data, copy_length);
		
		return r;
	}

} // namespace ipstack

