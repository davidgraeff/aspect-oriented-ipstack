#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::finwait1(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:  
    if(handleRST(segment)){ return; }
    if(handleSYN(segment)){ return; }

    //calculate payload length
    unsigned payload_len = len - (segment->get_header_len()*4);
    UInt32 seqnum = segment->get_seqnum();
    UInt32 acknum = segment->get_acknum();
    
    if(segment->has_ACK()){
      handleACK(acknum);
    }
    
    handleFIN(segment, seqnum, payload_len);
    bool needToFree = handleData(segment, seqnum, payload_len);
    
    //This segment ack'ed our FIN
    bool FIN_acked = (segment->has_ACK()) && (acknum == seqnum_next);
    
    if(needToFree == true){
      free(segment);
    }
    
    // **********************************************
    if(FIN_complete()){
      //we have received all data so far      
      if(FIN_acked == true){
        // Our FIN got ack'ed --> connection fully terminated now
        //printf("FIN+ACK arrived: FINWAIT1 --> TIMEWAIT\n");
        state = TIMEWAIT;
      }
      else{
        //printf("FIN arrived: FINWAIT1 --> CLOSING (simulaneous close)\n");
        state = CLOSING; //transition: FINWAIT1 --> CLOSING (simulaneous close)
      }
    }
    // **********************************************    
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    processACK(); //send ACK if requested
    if(history.isEmpty()){ //TODO: is this a good indicator? [ above: if(FIN_acked){ state = FINWAIT2; } ]
      // Our FIN got ack'ed
      //printf("ACK arrived: FINWAIT1 --> FINWAIT2\n");
      state = FINWAIT2;
    }
    else{
      //resend FIN after timeout
      block(history.getNextTimeout());
    }
  }
}

} //namespace ipstack
