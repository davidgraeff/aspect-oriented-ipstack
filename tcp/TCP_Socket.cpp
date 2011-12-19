// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


#include "TCP_Socket.h"

namespace ipstack {

void TCP_Socket::input(TCP_Segment* segment, unsigned len){
  // *THE* state machine switch
  switch(state){
    case CLOSED: closed(segment, len); break;
    case LISTEN: listen(segment, len); break;
    case SYNSENT: synsent(segment, len); break;
    case SYNRCVD: synrcvd(segment, len); break;
    case ESTABLISHED: established(segment, len); break;
    case FINWAIT1: finwait1(segment, len); break;
    case FINWAIT2: finwait2(segment, len); break;
    case CLOSEWAIT: closewait(segment, len); break;
    case LASTACK: lastack(segment, len); break;
    case CLOSING: closing(segment, len); break;
    case TIMEWAIT: timewait(segment, len); break;
  }
}

UInt16 TCP_Socket::getReceiveWindow(){
  if(state == SYNSENT || state == CLOSED){
    return (UInt16)TCP_Segment::DEFAULT_MSS; //no mss negotiated so far
  }
  else{
    unsigned currRecvWnd = maxReceiveWindow_Bytes - receiveBuffer.getRecvBytes();
    if(currRecvWnd > 0xFFFFU){
      //advertised window is only 16 bit wide
      return 0xFFFFU;
    }
    else{
      return (UInt16)currRecvWnd;
    }
  }
}

void TCP_Socket::updateSendWindow(TCP_Segment* segment, UInt32 seqnum, UInt32 acknum){
  if(TCP_Segment::SEQ_LT(seqnum, lwseq)){
    return; // this segment arrived out-of-order
    //we aleary have a newer window update
  }
  if( (seqnum == lwseq) &&
     TCP_Segment::SEQ_LT(acknum, lwack)) {
    //this acknowledgment arrived out-of-order
    //we already have received a more recent acknowledgment
    return;
  }
  //else, we have a send window update:
  
  sendWindow = segment->get_window();
  lwseq = seqnum;
  lwack = acknum;
}

void TCP_Socket::clearHistory(){
  while(TCP_Record* record = history.get()){
    free(record->getSegment());
    history.remove(record);
  }
}

void TCP_Socket::updateHistory(bool do_retransmit){
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
      if( (do_retransmit == true) && record->isTimedOut() ){
        //a retransmission timeout was reached
        retransmit(record, segment);
      }
      record = record->getNext();
    }
  }
}

void TCP_Socket::handleACK(UInt32 acknum){
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

bool TCP_Socket::FIN_complete(){
  //return true if a FIN was received and all data was received completely, too
  if((FIN_received == true) && (receiveBuffer.getAckNum() == FIN_seqnum)){
    sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
    return true;
  }
  return false;
}

bool TCP_Socket::handleSYN(TCP_Segment* segment){
  //This function is called only at: ESTABLISHED
  if(segment->has_SYN()){
    // 1) ACK of 'three way handshake' got lost! // && segment->has_ACK()){
    // 2) OR: //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
    free(segment);
    sendACK(); // 'retransmit'
    return true;
  }
  return false;
}

bool TCP_Socket::handleSYN_final(TCP_Segment* segment){
  //call this function if a FIN was received
  //This function is called at: CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT
  if(segment->has_SYN()){
    //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
    //TODO: "Internetworking with TCP/IP II page 202": answer a SYN with RST and abort()
    free(segment);
    sendACK(FIN_seqnum + 1U);
    return true;
  }
  return false;
}

bool TCP_Socket::handleRST(TCP_Segment* segment){
  if(segment->has_RST()){
    /*In all states except SYN-SENT, all reset (RST) segments are validated
    by checking their SEQ-fields.  A reset is valid if its sequence number
    is in the window. -> TODO */
    free(segment);
    abort(); //abort the connection
    return true; 
  }
  return false;
}

void TCP_Socket::processACK() {
  if(ACK_triggered == true){
    sendACK();
  }
}

void TCP_Socket::setupHeader(TCP_Segment* segment){
  segment->set_dport(dport);
  segment->set_sport(sport);
  segment->set_checksum(0);
  segment->set_seqnum(seqnum_next);
  //segment->set_acknum(0); //mostly overridden by caller. A packet without ACK is seldom used
  segment->set_header_len(TCP_Segment::TCP_MIN_HEADER_SIZE/4);
  segment->set_flags(0);
  segment->set_window(getReceiveWindow());
  segment->set_urgend_ptr(0);
}

void TCP_Socket::setupHeader_withACK(TCP_Segment* segment){
  setupHeader(segment);
  segment->set_ACK();
  segment->set_acknum(receiveBuffer.getAckNum());
}

void TCP_Socket::setMSS(unsigned max_segment_size) {
  mss = max_segment_size;
  
  if(sizeof(unsigned) == 2){
    //8 or 16 bit machine
    UInt32 maxRecvWnd = ((UInt32)maxReceiveWindow_MSS) * max_segment_size;
    if(maxRecvWnd > 0xFFFFU){
      //limit to 16 bit (65 KByte)
      maxReceiveWindow_Bytes = 0xFFFFU;
    }
    else{
      maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
    }
  }
  else{
    // 32 or 64 bit machine
    UInt64 maxRecvWnd = ((UInt64)maxReceiveWindow_MSS) * max_segment_size;
    if(maxRecvWnd > 0xFFFFFFFFUL){
      //limit to 32 bit (4 GByte)
      maxReceiveWindow_Bytes = (unsigned) 0xFFFFFFFFUL;
    }
    else{
      maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
    }
  }
}

void TCP_Socket::connect(){
  if(state == CLOSED){
    gen_initial_seqnum();
    sendWindow = 0;
    TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
    if(packet != 0){
      setupHeader(packet);
      packet->set_acknum(0); //clear ACK number ("a standard conforming implementation 
                             //must set ACK in all packets except for the initial SYN packet")
      seqnum_next++; // 1 seqnum consumed
      packet->set_SYN();
      state = SYNSENT; // next state
      send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
      waiting = true;
      history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO());
    }
  }
}

void TCP_Socket::abort(){
  //printf("Closing connection!\n");
  state = CLOSED;
  waiting = receiving = false;
  FIN_received = false;
  //TODO: clear receiveBuffer?
  //TODO: free & reset everything!!
  clearHistory(); //free all pending packets
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

bool TCP_Socket::block(UInt32 timeout) { return false; }

void TCP_Socket::block() {} //wait for incoming packets only

} //namespace ipstack
