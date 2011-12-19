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


#ifndef __UDP_SOCKET__
#define __UDP_SOCKET__

#include "../util/Mempool.h"
#include "UDP.h"
#include <string.h>

namespace ipstack {

class UDP_Socket{
  private:
  Mempool* mempool;
  UInt16 dport;
  UInt16 sport;
  unsigned network_header_offset; //Offset for IP-Header ( + Ethernet header)
  
  void setupHeader(UDP_Packet* packet, unsigned datasize){
    packet->set_dport(dport);
    packet->set_sport(sport);
    packet->set_length(datasize + UDP_Packet::UDP_HEADER_SIZE);
    packet->set_checksum(0);
  }
  
  public:
  
  void set_Mempool(Mempool* m) { mempool = m; }
  Mempool* get_Mempool() { return mempool; }
  
  void set_dport(UInt16 d) { dport = d; }
  UInt16 get_dport() { return dport; }
  
  void set_sport(UInt16 s) { sport = s; }
  UInt16 get_sport() { return sport; }  
  
  
  protected:
  //do not allow construction: only derived classes may be instantiated
  UDP_Socket() : mempool(0), dport(UDP_Packet::UNUSED_PORT),
                 sport(UDP_Packet::UNUSED_PORT), network_header_offset(0) {}
  
  void set_network_header_offset(unsigned offset) { network_header_offset = offset; }
  
  UDP_Packet* createPacket(const void* data, unsigned datasize){
    UDP_Packet* packet = (UDP_Packet*) alloc(datasize + UDP_Packet::UDP_HEADER_SIZE);
    if(packet != 0){
      memcpy(packet->get_data(), data, datasize);
      setupHeader(packet, datasize);
      return packet;
    }
    return 0;
  }
  
  void* alloc(unsigned datasize){
    void* buffer = mempool->alloc(datasize + network_header_offset);
    if(buffer != 0){
      return (((UInt8*)buffer) + network_header_offset);
    }
    else{
      return 0;
    }
  }
  
  void free(void* packet){
    mempool->free(packet); //new Mempool allows not-aligned frees ;-)
  }
  
};

} //namespace ipstack

#endif // __UDP_SOCKET__ 
