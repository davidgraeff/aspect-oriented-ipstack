#ifndef __IPV4_UDP_SOCKET__
#define __IPV4_UDP_SOCKET__

#include "../IPv4.h"
#include "../IPv4_Socket.h"
#include "../../udp/UDP_Socket.h"


namespace ipstack {

class IPv4_UDP_Socket : public IPv4_Socket, public UDP_Socket {
  public:
  IPv4_UDP_Socket();
  
};

} //namespace ipstack

#endif // __IPV4_UDP_SOCKET__ 
