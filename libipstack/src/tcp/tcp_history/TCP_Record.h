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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"
#include "../TCP_Segment.h"
#include "sending/SendBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{


class TCP_Record
{
	private:
		uint64_t timeout;
		SendBuffer* sendbuffer;

	public:
		TCP_Record* getNext() {
			return 0;    //overridden by TCP_History_SlidingWindow.ah
		}

		void setSendBuffer(SendBuffer* s) {
			sendbuffer = s;
		}
		SendBuffer* getSendBuffer() {
			return sendbuffer;
		}

		void setTimeout(uint32_t msec);

		uint64_t getTimeout() {
			return timeout;
		}

		bool isTimedOut() {
			if (timeout != 0) {
				return (Clock::now() > timeout);
			} else {
				return false;
			}
		}

		uint32_t getRemainingTime() {
			uint64_t currentTime = Clock::now();
			if (currentTime > timeout) {
				return 0;
				//is timed out
			} else {
				return Clock::ticks_to_ms(timeout - currentTime);
			}
		}

};

} // namespace ipstack
