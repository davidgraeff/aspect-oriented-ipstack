#ifndef __IPV4_TCP_SOCKET__
#define __IPV4_TCP_SOCKET__

#include "../IPv4.h"
#include "../IPv4_Socket.h"
#include "../../tcp/TCP_Socket.h"

#include <string.h> //for memcpy

namespace ipstack {

class IPv4_TCP_Socket : public IPv4_Socket, public TCP_Socket {
  private:
  IPv4_TCP_Socket* next;
  
  friend class Demux; //allow Demux to use getNext(), setNext(...)
  IPv4_TCP_Socket* getNext() { return next; }
  void setNext(IPv4_TCP_Socket* sock) { next = sock; }
  
  bool put(IPv4_Packet* packet, unsigned len){
    if(IPv4_Socket::get_packetbuffer()->isFull() == false){
      void* clone = TCP_Socket::get_Mempool()->alloc(len);
      if(clone != 0){
        memcpy(clone, packet, len);
        IPv4_Socket::get_packetbuffer()->put(clone);
        return true;
      }
    }
    return false;
  }
  
  void recv_loop(){
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
  
  // explicit join-points: affected by 'IPv4_TCP_Receive.ah' aspect.
  bool bind() { return false; } // register a new client socket at Demux
  void unbind() {} // remove this client socket at Demux
  
  public:
  IPv4_TCP_Socket() : TCP_Socket((IPv4_Socket*)this, IPv4_Socket::wrapper_send, IPv4_Socket::wrapper_hasBeenSent) {
    TCP_Socket::set_network_header_offset(IPV4_MIN_HEADER_SIZE);
  }

  bool connect(){
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
  
  bool close(){ //full close (no half-close supported)
    TCP_Socket::close();
    recv_loop();
    unbind();
    return TCP_Socket::isClosed();
  }
  
  bool send(const void* data, unsigned datasize){
    if( isEstablished() || isCloseWait() ){
      TCP_Socket::addSendData(data, datasize);
      recv_loop();
      return true;
    }
    return false;
  }
  
  int receive(void* buffer, unsigned buffer_len){
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
  
};

} //namespace ipstack

#endif // __IPV4_TCP_SOCKET__ 

