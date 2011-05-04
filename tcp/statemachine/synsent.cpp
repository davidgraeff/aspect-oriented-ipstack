#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::synsent(TCP_Segment* segment, unsigned len){
  if(segment != 0){
    // new tcp segment received:
    // waiting for SYN+ACK
    
    //extract everything we will need later
    UInt32 seqnum = segment->get_seqnum();
    UInt32 acknum = segment->get_acknum();
    UInt16 window = segment->get_window();
    bool segment_has_ACK = segment->has_ACK();
    bool segment_has_SYN = segment->has_SYN();
    bool segment_has_RST = segment->has_RST();
    free(segment); //and delete this packet
    
    if(segment_has_ACK){
      if(segment_has_SYN){
        handleACK(acknum);
        if(seqnum_unacked == seqnum_next){
          //acknum was correct
          state = ESTABLISHED;
          initSendWindow(window, seqnum, acknum);
          receiveBuffer.setFirstSeqNum(seqnum + 1U);
          
          if(sendACK()){
            waiting = false; //TODO: do that in established? (to clean up the ringbuffer)
          }
          return;
        }
      }
      else if(segment_has_RST){
        //"In the SYN-SENT state (a RST received in response
        //to an initial SYN), the RST is acceptable if the ACK field
        //acknowledges the SYN." (rfc793 page 37, Reset Processing)
        if(acknum == seqnum_next){
          abort(); //RST is valid
        }
        return;
      }
      //[else]
      //acknum incorrect: send a RST with seqNum = (incorrect ackNum)
      //rfc793 page 33 (Figure 9.) ("Recovery from Old Duplicate SYN")
      //-- AND --
      //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
      //Send a RST an continue connecting (resend SYN after its timeout)
      TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
      if(packet != 0){
        setupHeader(packet);
        packet->set_acknum(0); //clear ACK number (setupHeader() doesn't do that anymore)
        packet->set_seqnum(acknum);
        packet->set_RST();
        send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
        history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, 0); //no retransmission
      }
    }
    else if(segment_has_SYN){
      //rfc793 page 32 (Figure 8.) ("Simultaneous Connection Synchronization")
      //printf("SYN arrived -> Simultanous open !?\n");
      clearHistory(); //delete our SYN packet
      
      //borrowed form listen.cpp:
      receiveBuffer.setFirstSeqNum(seqnum + 1U);
      
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
    }
    //else? are there any other corner-cases? --> ignore
  }
  else{
    // there are no more segments in the input buffer
    updateHistory();
    block(history.getNextTimeout()); //wait for incoming packet OR timeout
  }
}

} //namespace ipstack
