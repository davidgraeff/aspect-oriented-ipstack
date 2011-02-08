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
  
  void free(UDP_Packet* packet){
    //void* buffer = (((UInt8*)packet) - network_header_offset);
    //mempool->free(buffer);
    //new Mempool allows not-aligned frees ;-)
    mempool->free(packet);
  }
  
};

} //namespace ipstack

#endif // __UDP_SOCKET__ 
