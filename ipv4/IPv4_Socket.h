#ifndef __IPV4_SOCKET__
#define __IPV4_SOCKET__

#include "util/types.h"
#include "../util/Ringbuffer.h"
#include "../router/Interface.h"
#include "IPv4.h"

namespace ipstack {
  
class IPv4_Socket{
  private:
  Packetbuffer* packetbuffer; //for receiving packets
  UInt32 dst_ipv4_addr;
  Interface* interface; //for sending packets
  
  UInt16 id;
  
  void setupInterface();
  void setupHeader(IPv4_Packet* packet, unsigned datasize, UInt8 protocol_id);

  protected:
  //do not allow construction: only derived classes may be instantiated
  IPv4_Socket() : packetbuffer(0), dst_ipv4_addr(0), interface(0), id(0) {}
  
  void send(void* data, unsigned datasize, UInt8 protocol_id);
  bool hasBeenSent(const void* data); 
  IPv4_Packet* receive();
  IPv4_Packet* read(); //non-blocking
  unsigned getMTU();
  bool hasValidInterface() { return (interface != 0); }
  
  public:
  void set_dst_ipv4_addr(UInt32 dst){
    dst_ipv4_addr = dst;
    setupInterface();
  }
  void set_dst_ipv4_addr(UInt8 a, UInt8 b, UInt8 c, UInt8 d){
    set_dst_ipv4_addr(IPv4_Packet::convert_ipv4_addr(a,b,c,d));
  }
  
  UInt32 get_dst_ipv4_addr() { return dst_ipv4_addr; }
  
  UInt32 get_src_ipv4_addr() {
    if(interface != 0) {
      return interface->getIPv4Addr();
    }
    return 0;
  }
  
  void set_packetbuffer(Packetbuffer* pb) { packetbuffer = pb; }
  Packetbuffer* get_packetbuffer() { return packetbuffer; }
  
  protected:
  // ***************************************************************************************
  static void wrapper_send(void* socket, void* data, unsigned datasize, UInt8 protocol_id){
    ((IPv4_Socket*)socket)->send(data, datasize, protocol_id);
  }
  static bool wrapper_hasBeenSent(void* socket, const void* data){
    return ((IPv4_Socket*)socket)->hasBeenSent(data);
  }
  // ***************************************************************************************
};


} //namespace ipstack

#endif /* __IPV4_SOCKET__ */
