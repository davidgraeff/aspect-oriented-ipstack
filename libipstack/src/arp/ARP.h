// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


#ifndef __ARP_H__
#define __ARP_H__

#include "util/ipstack_inttypes.h"

namespace ipstack {

class ARP_Packet {
  public:
  enum{ ETH_TYPE_ARP = 0x0806,
        ARP_HW_TYPE_ETH = 0x0001,
        ARP_REQUEST = 0x0001,
        ARP_REPLY = 0x0002,
        ARP_HEADER_SIZE = 8 };
       
  enum{ ARP_PROTOCOL_TYPE_IPV4 = 0x0800,
        ARP_HW_ADDR_SIZE_ETH = 6,
        ARP_PROTOCOL_ADDR_SIZE_IPV4 = 4,
        ARP_IPV4_PACKETSIZE = ARP_HEADER_SIZE + 2*ARP_HW_ADDR_SIZE_ETH + 2*ARP_PROTOCOL_ADDR_SIZE_IPV4 };

  private:
  uint16_t hw_type; //0x0001 for Ethernet
  uint16_t protocol_type; //0x0800 for IPv4
  uint8_t hw_addr_size; //6 bytes for Ethernet
  uint8_t protocol_addr_size; //4 bytes for IPv4
  uint16_t opcode; //0x0001 for Request, 0x0002 for Reply
  uint8_t payload[]; //src_hw_addr, src_proto_addr, dst_hw_addr, dst_proto_addr

  public:
  uint16_t get_hw_type() { return hw_type; }
  void set_hw_type(uint16_t t) { hw_type = t; }
  
  uint16_t get_protocol_type() { return protocol_type; }
  void set_protocol_type(uint16_t t) { protocol_type = t; }
  
  uint8_t get_hw_addr_size() { return hw_addr_size; }
  void set_hw_addr_size(uint8_t s) { hw_addr_size = s; }
  
  uint8_t get_protocol_addr_size() { return protocol_addr_size; }
  void set_protocol_addr_size(uint8_t s) { protocol_addr_size = s; }
  
  uint16_t get_opcode() { return opcode; }
  void set_opcode(uint16_t op) { opcode = op; }
  
  uint8_t* get_payload() { return payload; }
} __attribute__ ((packed));


} // namespace ipstack

#endif /* __ARP_H__ */
