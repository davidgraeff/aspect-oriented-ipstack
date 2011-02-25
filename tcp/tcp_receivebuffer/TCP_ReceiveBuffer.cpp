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

void TCP_ReceiveBuffer::socket_free(void* segment){
  socket->free(segment);
}

void TCP_ReceiveBuffer::copyData(void* dst, unsigned len){
  //no len checks here. caller is responsible!
  read_firstSeqNum += len;
  unsigned current_len = head->getLength();
  if(current_len == len){
    //consume head completely
    memcpy(dst, head->getData(), current_len);
    socket_free(head->getData()); //free @ TCP_Socket
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
