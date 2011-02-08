#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::synsent(TCP_Segment* segment, unsigned len){
  if(segment != 0){
    // new tcp segment received:
    // waiting for SYN+ACK
    if(segment->has_SYN() && segment->has_ACK()){
      UInt32 seqnum = segment->get_seqnum();
      UInt32 acknum = segment->get_acknum();
      handleACK(segment, acknum);
      if(seqnum_unacked == seqnum_next){
        //acknum was correct
        state = ESTABLISHED;
        initSendWindow(segment, seqnum, acknum);
        receiveBuffer.setFirstSeqNum(segment->get_seqnum() + 1U);
        free(segment); //delete this packet
        if(sendACK()){
          waiting = false; //TODO: do that in established? (to clean up the ringbuffer)
        }
      }
      else{
        //acknum incorrect: send a RST with seqNum = (incorrect ackNum)
        //rfc793 page 33 (Figure 9.) ("Recovery from Old Duplicate SYN")
        free(segment);
        TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
        if(packet != 0){
          setupHeader(packet);
          packet->set_seqnum(acknum);
          packet->set_RST();
          send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
          history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, 0); //no retransmission
        }
      }
    }
    else{
      synsent_handle_other(segment, len);
    }
  }
  else{
    // there are no more segments in the input buffer
    updateHistory();
    block(history.getNextTimeout()); //wait for incoming packet OR timeout
  }
}

void TCP_Socket::synsent_handle_other(TCP_Segment* segment, unsigned len){
  if(segment->has_RST() && segment->has_ACK()){
    //"In the SYN-SENT state (a RST received in response
    //to an initial SYN), the RST is acceptable if the ACK field
    //acknowledges the SYN." (rfc793 page 37, Reset Processing)
    if(segment->get_acknum() == seqnum_next){ //TODO: acknum parameter?
      //RST is valid
      //printf("RST in state SYNSENT arrived\n");
      free(segment);
      abort();
      return;
    }
    free(segment); //delete this packet
    return;
  }
  if(segment->has_SYN()){
    //rfc793 page 32 (Figure 8.) ("Simultaneous Connection Synchronization")
    //printf("SYN arrived -> Simultanous open !?\n");
    clearHistory(); //delete our SYN packet
    
    //borrowed form listen.cpp:
    receiveBuffer.setFirstSeqNum(segment->get_seqnum() + 1U);
    
    free(segment);
    
    //send SYN+ACK:
    TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
    if(packet != 0){
      state = SYNRCVD; //next state only if SYN+ACK can be sent (packet alloc'ed)
      setupHeader_withACK(packet);
      packet->set_seqnum(seqnum_next-1U); //TODO: correct?
      packet->set_SYN();
      send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
      history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO());
    }
    return;
  }
  if(segment->has_ACK()){
    //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
    //Send a RST an continue connecting (resend SYN after its timeout)
    UInt32 incorrect_ackNum = segment->get_acknum(); //TODO: acknum parameter
    free(segment);
    TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
    if(packet != 0){
      setupHeader(packet);
      packet->set_seqnum(incorrect_ackNum);
      packet->set_RST();
      send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
      history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, 0); //no retransmission
    }
    return;
  }
  //else? are there any other corner-cases? --> ignore
  free(segment);
}

} //namespace ipstack
