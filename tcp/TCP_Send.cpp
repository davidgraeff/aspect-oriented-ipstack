#include "TCP_Socket.h"
#include <string.h>

namespace ipstack {
  
void TCP_Socket::processSendData(){
  if(application_buflen > 0){
    //there is data to be sent!
    
    if(sendWindow == 0){ // (!) do NOT use getSendWindow(), because it may be affected by congestion avoidance aspects!
      //sendWindow is closed. we cannot send any data
      //probe for 'window update'
      //send a 'linux-style' zero window probe
      //(sequence number one less than expected, no data)
      if(history.getNextTimeout() == 0){
        //only probe for zero window if there is no other packet
        //in history which triggers an timeout event soon
        TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
        if(packet != 0){
          setupHeader_withACK(packet);
          packet->set_seqnum(seqnum_next-1U); // one less than expected
          waiting = true;
          send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
          //TODO: no R1, R2 value for zero window probes:
          //"Allow window stay zero indefinitely"
          history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO()); //TODO: Timeout of persist timer??
        }
      }
      return;
    }
    
    else{
      //sendWindow is opened
      while( (application_buflen > 0) && (getSendWindow() > 0) ){
        //TODO: propagate sendWindow below?!
        //we have data be be sent AND receiver can handle new packets
        if(sendNextSegment() == false){
          //stop further sending. an error (e.g. no buffer, silly window, ...) occured!
          return;
        }
      }
    }
  }
}

bool TCP_Socket::sendNextSegment(){
  //calculate length of packet to be send as the following minimum:
  UInt16 len = min(getSendWindow(), application_buflen, mss);
  
  TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE + len);
  if(packet != 0){
    setupHeader_withACK(packet);
    seqnum_next += len;
    
    //payload:
    memcpy(packet->get_data(), application_buf, len);
  
    lowerSendWindow(len); //subtract len bytes from sendWindow
    application_buflen -= len;
    application_buf = (void*) ( ((UInt8*)application_buf) + len );
  
    //set PUSH flag on last segment
    if(application_buflen == 0){
      packet->set_PSH();
    }
      
    send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE + len);
    history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE + len, getRTO());
    return true;
  }
  return false; //can't get buffer
}


} //namespace ipstack
