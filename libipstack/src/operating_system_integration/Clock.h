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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff


#ifndef __IPSTACK_CLOCK__
#define __IPSTACK_CLOCK__

#include "util/types.h"

namespace ipstack {

class Clock {
public:
	// Return a "tick" value, that can be used to measure time.
  static inline UInt64 now() {}
  static inline UInt64 ms_to_ticks(UInt32 ms) {}
  static inline UInt32 ticks_to_ms(UInt64 ticks) {}
};

} // ipstack

#endif /* __IPSTACK_CLOCK__ */
