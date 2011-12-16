#include "IPv4_Socket.h"
#include "ipstack/router/Router.h"

namespace ipstack {
  
void IPv4_Socket::setupInterface(){
  Router& router = Router::Inst(); //short name
  interface = router.ipv4_find_route(dst_ipv4_addr);
  if(interface == 0){
    //use default gateway
    interface = router.ipv4_find_route(router.ipv4_get_gateway_addr());
  }
}

void IPv4_Socket::setupHeader(IPv4_Packet* packet, unsigned datasize, UInt8 protocol_id){
  packet->set_ihl(IPv4_Packet::IPV4_MIN_HEADER_SIZE/4);
  packet->set_version(IPv4_Packet::IPV4_VERSION);
  packet->set_tos(IPv4_Packet::IPV4_DEFAULT_TOS);
  packet->set_total_len(datasize + IPv4_Packet::IPV4_MIN_HEADER_SIZE);
  packet->set_id(id++);
  packet->set_flags(IPv4_Packet::IPV4_DEFAULT_FLAGS);
  packet->set_fragment_offset(IPv4_Packet::IPV4_NO_FRAGMENT_OFFSET);
  packet->set_ttl(IPv4_Packet::IPV4_DEFAULT_TTL);
  packet->set_protocol(protocol_id);
  packet->set_src_ipaddr(interface->getIPv4Addr());
  packet->set_dst_ipaddr(dst_ipv4_addr);
    
  packet->computeChecksum(); //Insert IPv4 header checksum
}
  
void IPv4_Socket::send(void* data, unsigned datasize, UInt8 protocol_id){
  /*if(interface == 0){
    return;  // no route found
  }*/ //temporary commented out: can be removed at all if no further null pointer dereferences occur ;-)

  //be sure to reserve enough space in front of *data
  IPv4_Packet* packet = (IPv4_Packet*) (((UInt8*)data) - IPv4_Packet::IPV4_MIN_HEADER_SIZE);
  setupHeader(packet, datasize, protocol_id);

  //Ethernet aspect affecting this:
  interface->send(packet, datasize + IPv4_Packet::IPV4_MIN_HEADER_SIZE);
}

bool IPv4_Socket::hasBeenSent(const void* data){
  IPv4_Packet* packet = (IPv4_Packet*) (((UInt8*)data) - IPv4_Packet::IPV4_MIN_HEADER_SIZE);
  return interface->hasBeenSent(packet);
}

IPv4_Packet* IPv4_Socket::receive(){
  void* recv = 0;
  while(recv == 0){
    recv = packetbuffer->get();
  }
  return ((IPv4_Packet*) recv);
}

IPv4_Packet* IPv4_Socket::read(){
  return ((IPv4_Packet*) packetbuffer->get());
}

unsigned IPv4_Socket::getMTU(){
  if(interface != 0){
    //subtract IPv4 header size
    return (interface->getMTU() - IPv4_Packet::IPV4_MIN_HEADER_SIZE);
  }
  else{
    return 0;
  }
}

} //namespace ipstack

