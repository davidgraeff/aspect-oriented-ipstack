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

#pragma once

#include "util/ipstack_inttypes.h"
#include "../TCP_Segment.h"

namespace ipstack {

class ReceiveBuffer;
class TCP_RecvElement{
  private:
  unsigned len;
  uint32_t seqNum;
  uint8_t* data;
  ReceiveBuffer* memoryPtr;
  
  public:
  void setData(TCP_Segment* segment, unsigned length, uint32_t sequenceNumber){
    len = length;
    seqNum = sequenceNumber;
    data = segment->get_data();
	memoryPtr = (ReceiveBuffer*)((char*)segment - sizeof(ReceiveBuffer));
  }
  /**
   * The associated memory for this data. 
   */
  ReceiveBuffer* getMemoryPtr() {return segmentPtr;}

  void setLength(unsigned l) { len = l; }
  unsigned getLength() { return len; }
  
  void setSeqNum(uint32_t s) { seqNum = s; }
  uint32_t getSeqNum() { return seqNum; }
  
  uint8_t* getData() { return data; }
  void setData(uint8_t* d) { data = d; }
  
  uint32_t getNextSeqNum() { return (seqNum + len); }
  
  void increment(unsigned inc){
    data += inc;
    seqNum += inc;
    len -= inc;
  }
};

} // namespace ipstack
