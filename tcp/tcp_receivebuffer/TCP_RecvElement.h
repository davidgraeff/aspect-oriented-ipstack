#ifndef __TCP_RECVELEMENT_H__
#define __TCP_RECVELEMENT_H__

#include "util/types.h"
#include "../TCP.h"

namespace ipstack {

  
class TCP_RecvElement{
  private:
  unsigned len;
  UInt32 seqNum;
  UInt8* data;
  
  public:
  void setSegment(TCP_Segment* segment, unsigned length, UInt32 sequenceNumber){
    len = length;
    seqNum = sequenceNumber;
    data = segment->get_data();
  }
    
  void setLength(unsigned l) { len = l; }
  unsigned getLength() { return len; }
  
  void setSeqNum(UInt32 s) { seqNum = s; }
  UInt32 getSeqNum() { return seqNum; }
  
  UInt8* getData() { return data; }
  void setData(UInt8* d) { data = d; }
  
  UInt32 getNextSeqNum() { return (seqNum + len); }
  
  void increment(unsigned inc){
    data += inc;
    seqNum += inc;
    len -= inc;
  }
};

} // namespace ipstack

#endif // __TCP_RECVELEMENT_H__

