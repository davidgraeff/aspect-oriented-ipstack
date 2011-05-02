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
