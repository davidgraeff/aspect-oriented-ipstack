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


#ifndef __ETH_ARP_IPV4_PACKET__
#define __ETH_ARP_IPV4_PACKET__

#include "util/types.h"
#include "ipstack/ethernet/Eth_Frame.h"
#include "../ARP.h"
#include "ipstack/router/Interface.h"


namespace ipstack {

class Eth_ARP_IPv4_Packet {
  public:
  enum { HEADER_SIZE = Eth_Frame::ETH_HEADER_SIZE + ARP_Packet::ARP_HEADER_SIZE,
         FRAMESIZE = Eth_Frame::ETH_HEADER_SIZE + ARP_Packet::ARP_IPV4_PACKETSIZE };
    
  Eth_Frame* get_Eth_Frame(){ return (Eth_Frame*)header; }
  ARP_Packet* get_ARP_Packet() { return (ARP_Packet*) (header + Eth_Frame::ETH_HEADER_SIZE); }
  bool isFree(){
    return ((interface == 0) || (interface->hasBeenSent(this) == true));
  }
  
  //Ethernet + ARP header
  private:
  UInt8 header[HEADER_SIZE];
  
  public:
  //ARP Payload
  //Source (sender):
  UInt8 arp_src_hwaddr[ARP_Packet::ARP_HW_ADDR_SIZE_ETH];
  UInt32 arp_src_ipv4_addr;
  //Destination (receiver):
  UInt8 arp_dst_hwaddr[ARP_Packet::ARP_HW_ADDR_SIZE_ETH];
  UInt32 arp_dst_ipv4_addr;

  //The sending interface (for hasBeenSent(...))
  Interface* interface; //initialize to 0 (-> startup code)
} __attribute__ ((packed));

} // namespace ipstack

#endif // __ETH_ARP_IPV4_PACKET__
