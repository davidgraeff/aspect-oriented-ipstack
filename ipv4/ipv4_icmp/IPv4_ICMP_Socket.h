#ifndef __IPV4_ICMP_SOCKET__
#define __IPV4_ICMP_SOCKET__

#include "../IPv4.h"
#include "../IPv4_Socket.h"

namespace ipstack {

class IPv4_ICMP_Socket : public IPv4_Socket {

  public:
  IPv4_ICMP_Socket() {
  }
  
  bool send(ICMP_Packet* packet, unsigned datasize){
    if(packet != 0){

      IPv4_Socket::send(packet, datasize, ICMP_Packet::IPV4_TYPE_ICMP);
      while(!IPv4_Socket::hasBeenSent(packet)); // Sending is very fast, so polling is ok!
      return true;
    }
    return false;
  }
  
};

} //namespace ipstack

#endif // __IPV4_ICMP_SOCKET__ 
