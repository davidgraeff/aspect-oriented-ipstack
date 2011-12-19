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

