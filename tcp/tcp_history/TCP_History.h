#ifndef __TCP_HISTORY_H__
#define __TCP_HISTORY_H__

#include "util/types.h"
#include "../TCP.h"
#include "../TCP_Config.h"
#include "TCP_Record.h"

namespace ipstack {


class TCP_History{
  private:
  TCP_Record records[__TCP_HISTORY_MAX_PACKETS__];
  TCP_Record* head; //doubly linked (for quick remove) if TCP_History_SlidingWindow.ah

  public:
  TCP_History() : head(0) {}
  
  void add(TCP_Segment* segment, unsigned len, UInt32 msec){ //msec -> timeout for rexmit
    //be sure to check isFull _BEFORE_ using this function!
    
    head = &records[0];
    //fill in data
    head->setSegment(segment);
    head->setLength(len);
    head->setTimeout(msec);
  }
  
  TCP_Record* get() { return head; }
  bool isEmpty() { return (head == 0); }
  bool isFull() { return !isEmpty(); }
  
  void remove(TCP_Record* record){
    head = 0;
  }
  
  UInt32 getNextTimeout(){ // relative value in msecs
    if(head == 0){
      return 0;
    }
    else{
      return head->getRemainingTime();
    }
  }

};

} // namespace ipstack

#endif // __TCP_HISTORY_H__

