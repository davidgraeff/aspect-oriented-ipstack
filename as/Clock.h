// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


#ifndef __IPSTACK_CLOCK__
#define __IPSTACK_CLOCK__

#include "util/types.h"
#include "hw/hal/SystemClock.h"

namespace ipstack {

class Clock {
public:
  static inline UInt64 now() { return hw::hal::SystemClock::Inst().value(); }
  static inline UInt64 ms_to_ticks(UInt32 ms) { return ((hw::hal::SystemClock::Inst().freq() / 1000U) * (UInt64)ms); }
  static inline UInt32 ticks_to_ms(UInt64 ticks) { return (ticks / (hw::hal::SystemClock::Inst().freq() / 1000U)); }
};

} // ipstack

#endif /* __IPSTACK_CLOCK__ */
