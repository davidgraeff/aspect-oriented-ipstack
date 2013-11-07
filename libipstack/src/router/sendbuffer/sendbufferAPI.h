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

#include "util/ipstack_inttypes.h"
namespace ipstack
{

	/**
	* Inherit from this class if you want your socket to be extended by the sendbuffer API.
	* You need to have a method
	*   Interface* getUsedInterface();
	* implemented!
	*/
	class sendbufferAPI {
	public:
		/**
		* Send the data of a SendBuffer that was created with requestSendBuffer(). The packet is
		* invalid after sending it and may be freed by calling socket->freeSendbuffer(sendbuffer).
		*/
		static bool send(SendBuffer* dataToSend) {
			// it is not supported to send a sendbuffer again. Use sendbuffer->recyle().
			if (dataToSend->getState()!=SendBuffer::WritingState) {
				return false;
			}
			dataToSend->setState(SendBuffer::TransmittedState);
			dataToSend->getInterface()->send(dataToSend->getDataStart(), dataToSend->getSize());
			return true;
		}
	};
} // namespace ipstack
