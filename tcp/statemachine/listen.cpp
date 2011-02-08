#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::listen(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    
    //only SYN packet have come here.
    //do not check for SYN flag (once again).
    receiveBuffer.setFirstSeqNum(segment->get_seqnum() + 1U);
    
    free(segment);
    
    //send SYN+ACK:
    TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
    if(packet != 0){
      state = SYNRCVD; //next state only if SYN+ACK can be sent (packet alloc'ed)
      setupHeader_withACK(packet);
      seqnum_next++; // 1 seqnum consumed
      packet->set_SYN();
      send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
      history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO());
    }
  }
  else{
    // there are no more segments in the input buffer
    block(); //wait for SYN packet
  }
}

} //namespace ipstack

