#ifndef __TCP_RECORD_H__
#define __TCP_RECORD_H__

#include "util/types.h"
#include "../TCP.h"
#include "hw/hal/SystemClock.h"

namespace ipstack {

  
class TCP_Record{
  private:
  unsigned len;
  UInt64 timeout;
  TCP_Segment* segment;
  
  public:
  TCP_Record* getNext() { return 0; } //overridden by TCP_History_SlidingWindow.ah
  
  void setSegment(TCP_Segment* s){ segment = s; }
  TCP_Segment* getSegment() { return segment; }
  
  void setLength(unsigned l){ len = l; } //total lenght (including tcp header)
  unsigned getLength() { return len; } //total lenght (including tcp header)
  
  void setTimeout(UInt32 msec);
  
  UInt64 getTimeout() { return timeout; }

  bool isTimedOut() {
    if(timeout != 0){
      hw::hal::SystemClock& clock = hw::hal::SystemClock::Inst();
      return (clock.value() > timeout);
      //TODO: fuzzy timeout! (- some msecs)
      //-> e.g. clock.value() > (timeout & 0xFFFFFFFFFF0000ULL)
    }
    else{
      return false;
    }
  }
  
  UInt32 getRemainingTime(){
    hw::hal::SystemClock& clock = hw::hal::SystemClock::Inst();
    UInt64 currentTime = clock.value();
    if(currentTime > timeout){
      return 0;
      //is timed out
    }
    else{
      return (UInt32)( (timeout - currentTime) / (clock.freq() / 1000) );
    }
  }
  
};

} // namespace ipstack

#endif // __TCP_RECORD_H__

