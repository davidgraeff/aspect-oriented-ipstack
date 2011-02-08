#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::closing(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    if(handleSYN_final(segment)){ return; }
    
    UInt32 acknum = segment->get_acknum();
    handleACK(segment, acknum); //Just wait for an ACK (for our FIN)
    
    if(segment->has_FIN()){
      //Our ACK from state FINWAIT1 got lost. retransmit!
      sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
    }
    
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    if(history.isEmpty()){
      // Our FIN got ack'ed
      //printf("ACK arrived: CLOSING --> TIMEWAIT\n");
      state = TIMEWAIT;
    }
    else{
      //resend FIN (from close()) after timeout
      block(history.getNextTimeout());
    }
  }
}

} //namespace ipstack
