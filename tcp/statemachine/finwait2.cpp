#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::finwait2(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    
    if(handleRST(segment)){ return; }
    if(handleSYN(segment)){ return; }
    
    //calculate payload length
    unsigned payload_len = len - (segment->get_header_len()*4);
    UInt32 seqnum = segment->get_seqnum();
    
    handleFIN(segment, seqnum, payload_len);
    bool needToFree = handleData(segment, seqnum, payload_len);
    
    if(needToFree == true){
      free(segment);
    }
    
    // **********************************************
    if(FIN_complete()){
      //we have received all data so far
      //printf("FIN arrived: FINWAIT2 --> TIMEWAIT\n");
      state = TIMEWAIT;
    }
    // **********************************************    
  }
  else{
    // there are no more segments in the input buffer
    updateHistory();
    processACK(); //send ACK if requested
    block();
    //TODO: optional FIN_WAIT2_Timeout!! -> Aspect?
    //No support for half-close, because an incoming FIN must be handled
  }
}

} //namespace ipstack
