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


#include "TCP_ReceiveBuffer.h"
#include "../TCP_Socket.h"

namespace ipstack {

UInt32 TCP_ReceiveBuffer::getAckNum(){
  if(head == 0){
    return read_firstSeqNum;
  }
  else{
    return head->getNextSeqNum();
  }
}

void TCP_ReceiveBuffer::socket_free(TCP_Segment* segment){
	socket->freeReceivedSegment(segment);
}

void TCP_ReceiveBuffer::copyData(void* dst, unsigned len){
  //no len checks here. caller is responsible!
  read_firstSeqNum += len;
  unsigned current_len = head->getLength();
  if(current_len == len){
    //consume head completely
    memcpy(dst, head->getData(), current_len);
    socket_free(head->getSegmentPtr()); //free @ TCP_Socket
    head = 0;
    pushFlag = false; // Receive Buffer is empty now
  }
  else{
    //consume head only partial
    memcpy(dst, head->getData(), len);
    head->increment(len);
  }
} 
  
//insert returns 'true' if segment has been added successfully
//caller must free this packet if it returns 'false'
bool TCP_ReceiveBuffer::insert(TCP_Segment* segment, UInt32 seqnum, unsigned len){
  if(seqnum == read_firstSeqNum){
    //packet is in order
    if(head == 0){
      //there is a free buffer slot (the only one)
      head = &elements[0];
      head->setSegment(segment, len, seqnum);
      if(segment->has_PSH()) { pushFlag = true; }
      return true;
    }
  }
  return false;
}

} // namespace ipstack
