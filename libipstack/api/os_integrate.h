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

/**
 * Only use the provided API of this file if you've build libipstack with the option
 * BUILD_WITH_ASPECTLESS_INTERFACE.
 * This file contains API for clock and system_halt. You have to implement
 * all methods of this file.
 */

namespace IP {
	class Clock {
		public:
		// Return a "tick" value, that can be used to measure time.
		static UInt64 now();
		static UInt64 ms_to_ticks(UInt32 ms);
		static UInt32 ticks_to_ms(UInt64 ticks);
	};
	
	class System {
		public:
		/**
		* The ipstack calls this method to halt the system on fatal errors
		* (should not happen in normal operation mode and only on memory
		* corruption). On an x86 OS you would use
		*    asm volatile("cli\nhlt");
		* for example. On a userspace program you would call exit().
		*/
		static void haltsystem();
	};
}