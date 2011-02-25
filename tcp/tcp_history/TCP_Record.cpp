#include "TCP_Record.h"

namespace ipstack {

void TCP_Record::setTimeout(UInt32 msec) {
  if(msec != 0){
    hw::hal::SystemClock& clock = hw::hal::SystemClock::Inst();
    timeout = clock.value() + ( (clock.freq() / 1000) * (UInt64)msec );
  }
  else{
    timeout = 0;
  }
}

} // namespace ipstack

