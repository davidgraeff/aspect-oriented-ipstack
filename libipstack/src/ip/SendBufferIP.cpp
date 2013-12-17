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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "SendBuffer.h"
namespace ipstack {
	SendBuffer* SendBufferIP::requestIPBuffer(SocketMemory& mem, IP& ip, uint_fast16_t requestedSize,
													   ReceiveBuffer* use_as_response) {
		Interface* interface = (use_as_response) ? buffer->get_interface() : ip.getUsedInterface();
		SendBuffer* sendbuffer = SendBuffer::requestRawBuffer(mem, interface, requestSize);
		if (!sendbuffer) return 0;
		prepareSendBuffer(sendbuffer, ip, use_as_response);
		return sendbuffer;
	}
	
	void SendBufferIP::prepareSendBuffer(SendBuffer* sendbuffer, IP& ip, ReceiveBuffer* use_as_response) {
	}
};

} // namespace ipstack
