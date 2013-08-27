// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


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
  TCP_Segment* segmentPtr;
  
  public:
  void setSegment(TCP_Segment* segment, unsigned length, UInt32 sequenceNumber){
    len = length;
    seqNum = sequenceNumber;
    data = segment->get_data();
	segmentPtr = segment;
  }
  /**
   * The associated segment for this data. This is neccessary to know for freeing this
   * memory later on.
   */
  TCP_Segment* getSegmentPtr() {return segmentPtr;}
    
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

