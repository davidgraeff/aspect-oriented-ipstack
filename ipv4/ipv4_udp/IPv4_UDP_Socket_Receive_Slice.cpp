#include "IPv4_UDP_Socket.h"


namespace ipstack {

// explicit join-points: affected by 'IPv4_UDP_Receive.ah' aspect.
bool IPv4_UDP_Socket::bind() { return false; } // register a new udp socket at Demux
void IPv4_UDP_Socket::unbind() {} // remove this udp socket at Demux

} //namespace ipstack

