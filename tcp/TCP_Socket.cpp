#include "TCP_Socket.h"

namespace ipstack {

void TCP_Socket::updateHistory(){
  //remove TCP segments which are no longer useful from TCP_History
  //and retransmit timed out segements
  TCP_Record* record = history.get();
  while(record != 0){
    TCP_Segment* segment = record->getSegment();
    UInt64 timeout = record->getTimeout();
    if( ((timeout != 0) && (TCP_Segment::SEQ_LT(segment->get_seqnum(), seqnum_unacked))) ||
        ((timeout == 0) && hasBeenSent(segment)) ){            
      // 1) Segment is ack'ed (sequence number) OR
      // 2) Segment has no retransmission timeout and has left the network interface
      TCP_Record* next = record->getNext();
      free(segment);
      history.remove(record);
      record = next;
    }
    else{
      if(record->isTimedOut()){
        //a retransmission timeout was reached
        retransmit(record, segment);
      }
      record = record->getNext();
    }
  }
}

void TCP_Socket::handleACK(TCP_Segment* segment, UInt32 acknum){
  if(segment->has_ACK()){
    //this segments contains an acknowledgement number
    if(TCP_Segment::SEQ_LEQ(acknum, seqnum_unacked)){
      //acknum should be at least (seqnum_unacked+1)
      //printf("duplicate ACK arrived\n");
      return; //duplicate ACK
    }
    if(TCP_Segment::SEQ_GT(acknum, seqnum_next)){
      //acknum must be smaller or equal to seqnum_next
      //printf("ACK outside window arrived!\n");
      if(state == SYNRCVD){
        //TODO: send a reset (packet with RST flag, ACK?)
      }
      else{
        //ACK outside window //TODO
      }
      return;
    }
    set_seqnum_unacked(acknum); // seqnums < seqnum_unacked are ack'ed now
  }
}

bool TCP_Socket::handleData(TCP_Segment* segment, UInt32 seqnum, unsigned payload_len){
  bool needToFree = true; //caller must free this segment?
  if(payload_len > 0){
    // this segment contains payload data
    needToFree = (receiveBuffer.insert(segment, seqnum, payload_len) == false);
    triggerACK();
  }
  return needToFree;
}

bool TCP_Socket::sendACK(UInt32 ackNum){
  TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
  if(packet != 0){
    setupHeader(packet);
    packet->set_ACK();
    packet->set_acknum(ackNum);
    send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
    history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, 0); //no timeout, just an ACK
    return true;
  }
  return false;
}

void TCP_Socket::abort(){
  //printf("Closing connection!\n");
  state = CLOSED;
  waiting = receiving = false;
  FIN_received = false;
  //TODO: clear receiveBuffer?
  //TODO: free & reset everything!!
  //TODO: unbind??
  clearHistory(); //delete our SYN packet
}

void TCP_Socket::close(){
  if( (state == CLOSEWAIT) || (state == ESTABLISHED) ){
    //send FIN:
    TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
    if(packet != 0){
      setupHeader_withACK(packet);
      packet->set_FIN();
      seqnum_next++; // 1 seqnum consumed (by FIN)
      send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
      history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO());
      waiting = true;
      if(state == CLOSEWAIT){
        state = LASTACK; //passive close
      }
      else{ //if(state == ESTABLISHED)
        state = FINWAIT1; //active close
      }
    }
  }
  //else{
  //  printf("ERROR: close() must be in ESTABLISHED or CLOSEWAIT\n");
  //}
}

} //namespace ipstack
