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

/**
 * You have to implement the following classes
 */
namespace ipstack_app {

	class Clock {
	public:
		// Return a "tick" value, that can be used to measure time.
		static uint64_t now();
		static uint64_t ms_to_ticks(uint32_t ms);
		static uint32_t ticks_to_ms(uint64_t ticks);
	};

	class System {
	public:
		/**
		* The ipstack calls this method to halt the system on fatal errors
		* (should not happen in normal operation mode and only on memory
		* corruption).
		*/
		static void haltsystem();
	};
} // ipstack
