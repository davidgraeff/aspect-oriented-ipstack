#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::synrcvd(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    
    if(segment->has_ACK()){ 
      UInt32 acknum = segment->get_acknum();
      handleACK(acknum);
      if(acknum == seqnum_next){
        //This segment ack'ed our SYN
        //printf("ACK arrived: SYNRCVD --> ESTABLISHED\n");
        state = ESTABLISHED;
        initSendWindow(segment->get_window(), segment->get_seqnum(), acknum);
        clearHistory(); //no more packet to retransmit
        waiting = false; //TODO: do that in established? (to clean up the ringbuffer)
      }
    }
    else if(segment->has_SYN()){
      //Our SYN+ACK (from state LISTEN or SYNSENT) got lost
      TCP_Record* record = history.get();
      if(record != 0){
        //retransmit first item of TCP_History (there should be only one!)
        send(record->getSegment(), record->getLength());
      }
    }
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    //TODO: goto CLOSED after timeout (no R1, R2?)
    //for a timeout, we need another member variable :-(
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    block(history.getNextTimeout());
  }
}

} //namespace ipstack
