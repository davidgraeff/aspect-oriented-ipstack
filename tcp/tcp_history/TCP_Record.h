#ifndef __TCP_RECORD_H__
#define __TCP_RECORD_H__

#include "util/types.h"
#include "../TCP.h"
#include "../../Clock.h"

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
      return (Clock::now() > timeout);
    }
    else{
      return false;
    }
  }
  
  UInt32 getRemainingTime(){
    UInt64 currentTime = Clock::now();
    if(currentTime > timeout){
      return 0;
      //is timed out
    }
    else{
      return Clock::ticks_to_ms(timeout - currentTime);
    }
  }
  
};

} // namespace ipstack

#endif // __TCP_RECORD_H__

