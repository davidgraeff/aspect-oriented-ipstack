#include "TCP_Record.h"

namespace ipstack {

void TCP_Record::setTimeout(UInt32 msec) {
  if(msec != 0){
    timeout = Clock::now() + Clock::ms_to_ticks(msec);
  }
  else{
    timeout = 0;
  }
}

} // namespace ipstack

