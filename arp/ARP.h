#ifndef __ARP_H__
#define __ARP_H__

#include "util/types.h"

namespace ipstack {

//TODO: put enum into class ARP_Packet
enum { ETH_TYPE_ARP = 0x0806,
       ARP_HW_TYPE_ETH = 0x0001,
       ARP_REQUEST = 0x0001,
       ARP_REPLY = 0x0002,
       ARP_HEADER_SIZE = 8 };
       
enum { ARP_PROTOCOL_TYPE_IPV4 = 0x0800,
       ARP_HW_ADDR_SIZE_ETH = 6,
       ARP_PROTOCOL_ADDR_SIZE_IPV4 = 4,
       ARP_IPV4_PACKETSIZE = ARP_HEADER_SIZE + 2*ARP_HW_ADDR_SIZE_ETH + 2*ARP_PROTOCOL_ADDR_SIZE_IPV4 };


class ARP_Packet {
  private:
  UInt16 hw_type; //0x0001 for Ethernet
  UInt16 protocol_type; //0x0800 for IPv4
  UInt8 hw_addr_size; //6 bytes for Ethernet
  UInt8 protocol_addr_size; //4 bytes for IPv4
  UInt16 opcode; //0x0001 for Request, 0x0002 for Reply
  UInt8 payload[]; //src_hw_addr, src_proto_addr, dst_hw_addr, dst_proto_addr

  public:
  UInt16 get_hw_type() { return hw_type; }
  void set_hw_type(UInt16 t) { hw_type = t; }
  
  UInt16 get_protocol_type() { return protocol_type; }
  void set_protocol_type(UInt16 t) { protocol_type = t; }
  
  UInt8 get_hw_addr_size() { return hw_addr_size; }
  void set_hw_addr_size(UInt8 s) { hw_addr_size = s; }
  
  UInt8 get_protocol_addr_size() { return protocol_addr_size; }
  void set_protocol_addr_size(UInt8 s) { protocol_addr_size = s; }
  
  UInt16 get_opcode() { return opcode; }
  void set_opcode(UInt16 op) { opcode = op; }
  
  UInt8* get_payload() { return payload; }
} __attribute__ ((packed));


} // namespace ipstack

#endif /* __ARP_H__ */
