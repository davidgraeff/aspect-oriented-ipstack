#include "TCP_History.h"

namespace ipstack {

void TCP_History::add(TCP_Segment* segment, unsigned len, UInt32 msec){ //msec -> timeout for rexmit
  //be sure to check isFull _BEFORE_ using this function!
  
  head = &records[0];
  //fill in data
  head->setSegment(segment);
  head->setLength(len);
  head->setTimeout(msec);
}

void TCP_History::remove(TCP_Record* record){
  head = 0;
}

UInt32 TCP_History::getNextTimeout(){ // relative value in msecs
  if(head == 0){
    return 0;
  }
  else{
    return head->getRemainingTime();
  }
}

} // namespace ipstack

