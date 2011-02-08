#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::timewait(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    if(handleSYN_final(segment)){ return; }
    
    if(segment->has_FIN()){
      //Our ACK from laste state (FINWAIT1, FINWAIT2) got lost -> retransmit
      sendACK(FIN_seqnum + 1U);
    }
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    if(block(2*getRTO()) == true){
      //----
      //NOTE: This is NOT conform to rfc1122,
      //but since there is no concurrency here,
      //waiting 2 min (=2MSL) in userspace is quite a long time ...
      //Just waiting for 2*RTO seems to be sufficient.
      //----
      abort(); //kill connection
    }
    //No support for half-close, because an incoming FIN must be handled
  }
}

} //namespace ipstack
