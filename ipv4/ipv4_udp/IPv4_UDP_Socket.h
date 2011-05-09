#ifndef __IPV4_UDP_SOCKET__
#define __IPV4_UDP_SOCKET__

#include "../IPv4.h"
#include "../IPv4_Socket.h"
#include "../../udp/UDP_Socket.h"


namespace ipstack {

class IPv4_UDP_Socket : public IPv4_Socket, public UDP_Socket {
  public:
  IPv4_UDP_Socket() {
    UDP_Socket::set_network_header_offset(IPv4_Packet::IPV4_MIN_HEADER_SIZE);
  }
  
  // explicit join-points: affected by 'IPv4_UDP_Receive.ah' aspect.
  bool bind(); // register a new udp socket at Demux
  void unbind(); // remove this udp socket at Demux
  // the prototypes are placed here as a workaround to avoid inlining
};

} //namespace ipstack

#endif // __IPV4_UDP_SOCKET__ 
