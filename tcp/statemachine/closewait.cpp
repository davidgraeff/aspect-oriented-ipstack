#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::closewait(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    if(handleRST(segment)){ return; }
    if(handleSYN_final(segment)){ return; }
    
    UInt32 seqnum = segment->get_seqnum();
    UInt32 acknum = segment->get_acknum();
    
    if(segment->has_ACK()){
      handleACK(acknum);
    }
    
    if(segment->has_FIN()){
      //Our ACK from state ESTABLISHED got lost. retransmit!
      sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
    }
    
    updateSendWindow(segment, seqnum, acknum);
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    
    if(receiving == true){
      //There will be no more incoming data segments.
      // => flush receiveBuffer if possible
      unsigned recvBytes = receiveBuffer.getRecvBytes();
      if(application_buflen > recvBytes){
        application_buflen = recvBytes; //only copy recvBytes
      }
      application_recv_len += application_buflen; //increment recv_len byte count
      receiveBuffer.copyData(application_buf, application_buflen);
      receiving = waiting = false; //never block on reading in this state
      return;
      //TODO: urgend data?
    }
    else{
      //Sending (TODO: sending == true?)
      processSendData(); //try to send new segments if possible
      if( (application_buflen == 0) && history.isEmpty()){
        waiting = false;
        return;
      }
      else{
        block(history.getNextTimeout());
      }
    }
  }
}

} //namespace ipstack
