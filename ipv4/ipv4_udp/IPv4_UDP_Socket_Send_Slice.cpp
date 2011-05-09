#include "IPv4_UDP_Socket.h"

#include "../../udp/UDP.h"
#include "../../udp/UDP_Socket.h"
#include "../IPv4_Socket.h"

namespace ipstack {

bool IPv4_UDP_Socket::send(const void* data, unsigned datasize){
  UDP_Packet* packet = UDP_Socket::createPacket(data, datasize);
  if(packet != 0){
    IPv4_Socket::send(packet, datasize + UDP_Packet::UDP_HEADER_SIZE, UDP_Packet::IPV4_TYPE_UDP);
    while(!IPv4_Socket::hasBeenSent(packet)); // Sending is very fast, so polling is ok!
    UDP_Socket::free(packet);
    return true;
  }
  return false;
}

} //namespace ipstack

