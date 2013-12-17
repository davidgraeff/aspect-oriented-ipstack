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

#include "ip/RawIP_Socket.h"
#include "util/ipstack_inttypes.h"
#include "ip/SendbufferIP.h"

namespace ipstack {
	bool RawIP_Socket::send(char* data, int len, ReceiveBuffer* use_as_response) {
		SendBuffer* sendbuffer = SendbufferIP::requestIPBuffer(*this, *this, len, use_as_response);
		if (!sendbuffer)
			return false;
		sendbuffer->write(data, len);
		sendbuffer->send();
		freeSendbuffer(sendbuffer);
		return true;
	}
}