// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


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
