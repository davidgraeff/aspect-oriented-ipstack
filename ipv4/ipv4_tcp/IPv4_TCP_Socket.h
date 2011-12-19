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
  
  void recv_loop();
  
  // explicit join-points: affected by 'IPv4_TCP_Receive.ah' aspect.
  bool bind(); // register a new client socket at Demux
  void unbind(); // remove this client socket at Demux
  
  public:
  IPv4_TCP_Socket();
  ~IPv4_TCP_Socket(){ unbind(); } // remove this socket at Demux on destruction

  bool connect();
  
  bool close(); //full close (no half-close supported)
  
  bool send(const void* data, unsigned datasize);
  
  int receive(void* buffer, unsigned buffer_len);

  int poll(unsigned msec);
  
};

} //namespace ipstack

#endif // __IPV4_TCP_SOCKET__ 

