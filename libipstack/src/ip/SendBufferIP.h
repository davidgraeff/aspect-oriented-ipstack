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

#include "memory_management/MemoryInterface.h"
#include "router/Interface.h"
#include "util/ipstack_inttypes.h"
#include "util/protocol_layers.h"
#include "router/sendbuffer/SendBuffer.h"

namespace ipstack {
class SendBufferIP : public SendBuffer
{
	public:
		/**
		* Create a new SendBufferIP object and allocate memory for it. If not enough memory is available 0
		* is returned. You should provide the destination interface.
		* Influenced by aspects of all network-stack layers:
		*
		* "Before": To get the mimimum
		* size for a send buffer to contain all those headers. The current state
		* of the layers should be taken into consideration. E.g. IPv6 should
		* increase the size by its own header and all IPv6 extension headers it
		* plan to add in its current state.

		* "Around" (after): To write protocol headers before the payload.
		* The current state of the layers should be taken into consideration. E.g. IPv6 should
		* add its own header + IPv6 extension headers depending on its current state.
		*/
		static SendBuffer* requestIPBuffer(SocketMemory& mem, IP& ip, uint_fast16_t requestedSize,
											 ReceiveBuffer* use_as_response = 0);

	private:
		void prepareSendBuffer(SendBuffer* sendbuffer, IP& ip, ReceiveBuffer* use_as_response);
		explicit SendBufferIP(); // private constructor
		SendBufferIP(const SendBufferIP& s); // private copy constructor
};

} // namespace ipstack
