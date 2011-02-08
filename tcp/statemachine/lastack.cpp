#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::lastack(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    if(handleSYN_final(segment)){ return; }
    UInt32 acknum = segment->get_acknum();
    
    handleACK(segment, acknum); //Just wait for an ACK (for our FIN)
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    if(history.isEmpty()){
      // Our FIN got ack'ed --> connection fully terminated now
      abort(); //kill connection
    }
    else{
      //resend FIN(+ACK) after a timeout
      block(history.getNextTimeout());
    }
  }
}

} //namespace ipstack
