#include "IPv4_UDP_Socket.h"

namespace ipstack {

IPv4_UDP_Socket::IPv4_UDP_Socket() {
  UDP_Socket::set_network_header_offset(IPv4_Packet::IPV4_MIN_HEADER_SIZE);
}

} //namespace ipstack

