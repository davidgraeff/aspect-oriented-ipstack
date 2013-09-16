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

	ReceiveBuffer::ReceiveBuffer(char* data, uint16_t receivedSize) : m_data(data), m_receivedSize(receivedSize) {};
	
	bool ReceiveBuffer::clone(SocketMemory& socket_mem, void* start_mem = 0) {
		if (cloned)
			return false;
		
		// set start position for copy if not set
		if (!start_mem)
			start_mem = m_data;
		
		const int copy_length = m_receivedSize - (start_mem-m_data);
		ReceiveBuffer* r = (ReceiveBuffer*)socket_mem.get_Mempool()->alloc(copy_length + sizeof(ReceiveBuffer));
		if (!r)
			return false;
		
		r->cloned = true;
		r->protocol_pointer = protocol_pointer;
		r->m_data = mem.m_cloned_data;
		r->m_receivedSize = copy_length;
		memcpy(r->m_cloned_data, m_data, copy_length);
		
		return true;
	}

} // namespace ipstack

