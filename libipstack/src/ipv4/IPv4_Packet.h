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
#pragma once
#include "util/ipstack_inttypes.h"

namespace ipstack {

class IPv4_Packet{
  public:
  enum{ ETH_TYPE_IPV4 = 0x0800, //Ethernet type value, 0x86DD for ipv6 (rfc2464)
        IPV4_VERSION = 4,
        IPV4_MIN_HEADER_SIZE = 20,
        IPV4_DF_FLAG = 0x02,
        IPV4_MF_FLAG = 0x01 };
       
  enum{ IPV4_DEFAULT_TOS = 0,
        IPV4_DEFAULT_TTL = 64,
        IPV4_DEFAULT_FLAGS = 0,
        IPV4_NO_FRAGMENT_OFFSET = 0 };
      
  enum{ IPV4_UNUSED_ADDR = 0 }; // http://www.rfc-editor.org/rfc/rfc5735.txt

  private:
  uint8_t ihl:4,
        version:4;
  uint8_t tos; //type of service -> diff'serv
  uint16_t total_len;
  uint16_t id; //uniquely identifying fragments
  union{
    uint16_t flags; //upper 3 bits //TODO: uint8_t?
    uint16_t fragment_offset; //lower 13 bits (measured in units of eight-byte blocks)
  };
  uint8_t ttl;
  uint8_t protocol;
  uint16_t hdr_checksum;
  uint32_t src_ipaddr;
  uint32_t dst_ipaddr;
  //options -> data
  uint8_t data[];
  
  static const uint16_t FRAGMENT_MASK = 0xFF1F;
  
  private:
  //from: Internetworking with TCP/IP Vol. 2, Page 70
  uint16_t cksum(uint16_t* buf, int nwords){
    uint32_t sum;
    
    for(sum=0; nwords>0; nwords--){
      sum += *buf++;
    }
    sum = (sum >> 16) + (sum & 0xffff);  /* add in carry    */
    sum += (sum >> 16);                  /* may be one more */
    return ~sum;
    //returns 0 if checksum was correct
  }
  
  public:
	  uint16_t payload_len() { return get_total_len()-get_ihl()*4; } // no get_ to be not influenced by aspects
  uint16_t get_total_len() { return total_len; }
  void set_total_len(uint16_t len) { total_len = len; }
  
  uint8_t get_ihl() { return ihl; }
  void set_ihl(uint8_t i) { ihl = i; }
  
  uint8_t get_version() { return version; }
  void set_version(uint8_t ver) { version = ver; }
  
  uint8_t get_tos() { return tos; }
  void set_tos(uint8_t t) { tos = t; }
  
  uint16_t get_id() { return id; }
  void set_id(uint16_t i) { id = i; }
  
  uint8_t get_flags() { return ((flags >> 5) & 0x7); }
  void set_flags(uint8_t f) { 
    flags &= FRAGMENT_MASK; //clean up bits first
    flags |= ((f & 0x7) << 5) ;
  }
  
  uint16_t get_fragment_offset() { return (fragment_offset & FRAGMENT_MASK); }
  void set_fragment_offset(uint16_t fo) { 
    fragment_offset &= ~(FRAGMENT_MASK); //clean up bits first
    fragment_offset |= (fo & FRAGMENT_MASK);
  }
  
  uint8_t get_ttl() { return ttl; }
  void set_ttl(uint8_t t) { ttl = t; }
  
  uint8_t get_protocol() { return protocol; }
  void set_protocol(uint8_t proto) { protocol = proto; }
  char* get_nextheaderPointer() {
	  return (char*)&protocol;
  }
  
  uint16_t get_hdr_checksum() { return hdr_checksum; }
  void set_hdr_checksum(uint16_t csum) { hdr_checksum = csum; }
  
  uint32_t get_src_ipaddr() { return src_ipaddr; }
  void set_src_ipaddr(uint32_t src) { src_ipaddr = src; }
  
  uint32_t get_dst_ipaddr() { return dst_ipaddr; }
  void set_dst_ipaddr(uint32_t dst) { dst_ipaddr = dst; }
  
  uint8_t* get_data() {
    //remove ip header + ip options
    //do not return data[] directly because of variable header length
    return (uint8_t*) (((uint32_t*) this) + get_ihl());
  }
  
  unsigned validPacketLength(unsigned packet_len){
    //returns 0 if packet length is invalid
    //returns the valid ipv4 packet length otherwise
    
    if(packet_len < IPV4_MIN_HEADER_SIZE){
      //packet too small!
      return 0;
    }
    
    uint8_t ihl = get_ihl() * 4;
    //Internet Header Lenght == length of ipv4 header in 4 byte units
    if(ihl < IPV4_MIN_HEADER_SIZE){
      //bad ihl value
      return 0;
    }
    
    if(ihl > packet_len){
      //ipv4 header seems to be bigger than the packet
      return 0;
    }
    
    //Get length of ipv4 packet. Byte order conversion is
    //done by an aspect affecting "get_total_len()".
    //Safe this value to avoid redundant conversions.
    uint16_t total_len = get_total_len();
    
    if(total_len < ihl){
      //invalid packet size: entire packet smaller than header size
      return 0;
    }
    
    if(total_len > packet_len) {
      //ip paket size is bigger than the entire buffer!
      return 0;
    }
    
    return total_len;
  }

  bool validPacketChecksum(void*){
    return (cksum((uint16_t*)this, (get_ihl()*2)) == 0);
  }
  
  inline bool validPacketChecksum(){
    return (cksum((uint16_t*)this, (get_ihl()*2)) == 0);
  }
    
  inline void computeChecksum(){
    hdr_checksum = 0;
    hdr_checksum = cksum((uint16_t*)this, (get_ihl()*2));
  }
  
  void clearChecksum(){
    hdr_checksum = 0;
  }
  
  static uint32_t convert_ipv4_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
    uint32_t addr = (((uint32_t)(a & 0xFF))   )  |
                  (((uint32_t)(b & 0xFF))<<8)  |
                  (((uint32_t)(c & 0xFF))<<16) |
                  (((uint32_t)(d & 0xFF))<<24) ;
    return addr;
  }
  
  static void convert_ipv4_addr(uint32_t ip, uint8_t& a, uint8_t& b, uint8_t& c, uint8_t& d) {
	  a = ((uint8_t)(ip      )) & 0xFF;
	  b = ((uint8_t)(ip >>  8)) & 0xFF;
	  c = ((uint8_t)(ip >> 16)) & 0xFF;
	  d = ((uint8_t)(ip >> 24)) & 0xFF;
  }
  
} __attribute__ ((packed)); //__attribute__ ((aligned(1), packed));

} //namespace ipstack

