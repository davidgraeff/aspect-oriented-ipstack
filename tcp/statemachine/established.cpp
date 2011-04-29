#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::established(TCP_Segment* segment, unsigned len) {
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
    
    updateSendWindow(segment, seqnum, acknum);
    
    if(needToFree == true){
      free(segment);
    }
    
    // **********************************************
    if(FIN_complete()){
      //we have received all data so far
      //printf("FIN arrived: ESTABLISHED --> CLOSEWAIT\n");
      state = CLOSEWAIT; //transition: ESTABLISHED --> CLOSEWAIT
    }
    // **********************************************
    
    //respond to "keep-alive" persist window probes
    if(getReceiveWindow() == 0) { triggerACK(); }
    //TODO:
    /*"The proper behaviour of TCP is NOT to respond when it receives a 
    segment with no data and with the next expected sequence number. 
    However, Comer’s code WILL respond to such segment
    (function tcpackit(), p. 303). Zero window probes usually have
    the sequence number one less than expected (Linux) or the expected
    sequence number but only one byte of data (Windows, BSD)."*/
    // [http://simonf.com/tcp/]
    
  }
  else{
    // there are no more segments in the input buffer
    updateHistory(); //cleanup packets which are not used anymore (and trigger retransmissions)
    
    if(receiving == true){
      unsigned recvBytes = receiveBuffer.getRecvBytes();
      if(recvBytes > 0){
        bool pushed = receiveBuffer.isPushed(); //remember here, because copyData removes the pushFlag
        
        unsigned copyBytes = recvBytes > application_buflen ? application_buflen : recvBytes;
        receiveBuffer.copyData(application_buf, copyBytes);
        application_recv_len += copyBytes; //increment recv_len byte count
        application_buf = (void*) (((UInt8*)application_buf) + copyBytes); //adjust application_buf pointer
        application_buflen -= copyBytes; //adjust data to be still read
        
        processACK();
        
        if( (application_buflen == 0) || (pushed == true) ){
          //return data to the receiving application
          receiving = waiting = false;
          return;
        }
      }
      else{
        processACK();
      }
      //TODO: no buffering requested, urgend data?!
      block();
    }
    else{
      //Sending
      processACK();
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
