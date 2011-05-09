#include "IPv4_TCP_Socket.h"

namespace ipstack {

IPv4_TCP_Socket::IPv4_TCP_Socket() : TCP_Socket((IPv4_Socket*)this, IPv4_Socket::wrapper_send, IPv4_Socket::wrapper_hasBeenSent) {
  TCP_Socket::set_network_header_offset(IPv4_Packet::IPV4_MIN_HEADER_SIZE);
}

bool IPv4_TCP_Socket::connect(){
  if(IPv4_Socket::get_dst_ipv4_addr() == 0 || IPv4_Socket::get_src_ipv4_addr() == 0){
    return false; //no (valid) dst_ipv4_addr set
  }
  if(TCP_Socket::isClosed()){
    //only allow connecting in CLOSED state
    TCP_Socket::set_sport(TCP_Segment::UNUSED_PORT); //reset source port
    if(bind()){
      TCP_Socket::setMTU(IPv4_Socket::getMTU());
      TCP_Socket::connect();
      recv_loop();
      return TCP_Socket::isEstablished();
    }
  }
  return false;
}

bool IPv4_TCP_Socket::close(){ //full close (no half-close supported)
  TCP_Socket::close();
  recv_loop();
  unbind();
  return TCP_Socket::isClosed();
}

bool IPv4_TCP_Socket::send(const void* data, unsigned datasize){
  if( isEstablished() || isCloseWait() ){
    TCP_Socket::addSendData(data, datasize);
    recv_loop();
    return true;
  }
  return false;
}

void IPv4_TCP_Socket::recv_loop(){
  while(TCP_Socket::waiting_for_input()) {
    IPv4_Packet* packet = IPv4_Socket::read(); //non-blocking read
    if(packet != 0){
      unsigned len = packet->get_total_len() - packet->get_ihl()*4;
      TCP_Segment* segment = (TCP_Segment*) packet->get_data();
      TCP_Socket::input(segment, len);
    }
    else{
      TCP_Socket::input(0, 0);
    }
  }
}

int IPv4_TCP_Socket::receive(void* buffer, unsigned buffer_len){
  if( isEstablished() || isCloseWait() ){
    //zero out MSB, because return value is an int!
    const unsigned msb = 1 << ((8*sizeof(unsigned))-1);
    buffer_len &= ~msb;
    TCP_Socket::receive(buffer, buffer_len);
    recv_loop();
    unsigned recv_len = TCP_Socket::get_application_recv_len();
    if( (recv_len == 0) && isCloseWait() ){
      return -2;
    }
    return (int)recv_len;
  }
  return -1;
}

int IPv4_TCP_Socket::poll(unsigned msec){
  if( isEstablished() || isCloseWait() ){
    IPv4_Packet* packet = IPv4_Socket::read(); //non-blocking read
    if(packet == 0){
      //no gratuitous packets received, yet
      bool timeout_reached = TCP_Socket::block((UInt32)msec); //wait for max. msec
      if(timeout_reached == false) {
        packet = IPv4_Socket::read(); //check for new packet, since timeout was not reached
      }
    }
    if(packet != 0){
      do {
        //process all gratuitous packets that have arrived
        unsigned len = packet->get_total_len() - packet->get_ihl()*4;
        TCP_Segment* segment = (TCP_Segment*) packet->get_data();
        TCP_Socket::input(segment, len);
        packet = IPv4_Socket::read();
      } while(packet != 0);
      updateHistory(); //cleanup packets which are not used anymore
      processACK(); //send ACK if necessary
    }

    //return amount of received data bytes (payload) while polling
    //zero out MSB, because return value is an int!
    const unsigned msb = 1 << ((8*sizeof(unsigned))-1);
    return (int) (TCP_Socket::getRecvBytes() & ~msb);
  }
  return -1;
}

} //namespace ipstack 

