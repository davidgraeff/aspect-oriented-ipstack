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


#ifndef __IPV4__
#define __IPV4__

#include "util/types.h"

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
  UInt8 ihl:4,
        version:4;
  UInt8 tos; //type of service -> diff'serv
  UInt16 total_len;
  UInt16 id; //uniquely identifying fragments
  union{
    UInt16 flags; //upper 3 bits //TODO: UInt8?
    UInt16 fragment_offset; //lower 13 bits (measured in units of eight-byte blocks)
  };
  UInt8 ttl;
  UInt8 protocol;
  UInt16 hdr_checksum;
  UInt32 src_ipaddr;
  UInt32 dst_ipaddr;
  //options -> data
  UInt8 data[];
  
  static const UInt16 FRAGMENT_MASK = 0xFF1F;
  
  private:
  //from: Internetworking with TCP/IP Vol. 2, Page 70
  UInt16 cksum(UInt16* buf, int nwords){
    UInt32 sum;
    
    for(sum=0; nwords>0; nwords--){
      sum += *buf++;
    }
    sum = (sum >> 16) + (sum & 0xffff);  /* add in carry    */
    sum += (sum >> 16);                  /* may be one more */
    return ~sum;
    //returns 0 if checksum was correct
  }
  
  public:
	  UInt16 payload_len() { return get_total_len()-get_ihl()*4; } // no get_ to be not influenced by aspects
  UInt16 get_total_len() { return total_len; }
  void set_total_len(UInt16 len) { total_len = len; }
  
  UInt8 get_ihl() { return ihl; }
  void set_ihl(UInt8 i) { ihl = i; }
  
  UInt8 get_version() { return version; }
  void set_version(UInt8 ver) { version = ver; }
  
  UInt8 get_tos() { return tos; }
  void set_tos(UInt8 t) { tos = t; }
  
  UInt16 get_id() { return id; }
  void set_id(UInt16 i) { id = i; }
  
  UInt8 get_flags() { return ((flags >> 5) & 0x7); }
  void set_flags(UInt8 f) { 
    flags &= FRAGMENT_MASK; //clean up bits first
    flags |= ((f & 0x7) << 5) ;
  }
  
  UInt16 get_fragment_offset() { return (fragment_offset & FRAGMENT_MASK); }
  void set_fragment_offset(UInt16 fo) { 
    fragment_offset &= ~(FRAGMENT_MASK); //clean up bits first
    fragment_offset |= (fo & FRAGMENT_MASK);
  }
  
  UInt8 get_ttl() { return ttl; }
  void set_ttl(UInt8 t) { ttl = t; }
  
  UInt8 get_protocol() { return protocol; }
  void set_protocol(UInt8 proto) { protocol = proto; }
  char* get_nextheaderPointer() {
	  return (char*)&protocol;
  }
  
  UInt16 get_hdr_checksum() { return hdr_checksum; }
  void set_hdr_checksum(UInt16 csum) { hdr_checksum = csum; }
  
  UInt32 get_src_ipaddr() { return src_ipaddr; }
  void set_src_ipaddr(UInt32 src) { src_ipaddr = src; }
  
  UInt32 get_dst_ipaddr() { return dst_ipaddr; }
  void set_dst_ipaddr(UInt32 dst) { dst_ipaddr = dst; }
  
  UInt8* get_data() {
    //remove ip header + ip options
    //do not return data[] directly because of variable header length
    return (UInt8*) (((UInt32*) this) + get_ihl());
  }
  
  unsigned validPacketLength(unsigned packet_len){
    //returns 0 if packet length is invalid
    //returns the valid ipv4 packet length otherwise
    
    if(packet_len < IPV4_MIN_HEADER_SIZE){
      //packet too small!
      return 0;
    }
    
    UInt8 ihl = get_ihl() * 4;
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
    UInt16 total_len = get_total_len();
    
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
    return (cksum((UInt16*)this, (get_ihl()*2)) == 0);
  }
  
  inline bool validPacketChecksum(){
    return (cksum((UInt16*)this, (get_ihl()*2)) == 0);
  }
    
  inline void computeChecksum(){
    hdr_checksum = 0;
    hdr_checksum = cksum((UInt16*)this, (get_ihl()*2));
  }
  
  void clearChecksum(){
    hdr_checksum = 0;
  }
  
  static UInt32 convert_ipv4_addr(UInt8 a, UInt8 b, UInt8 c, UInt8 d){
    UInt32 addr = (((UInt32)(a & 0xFF))   )  |
                  (((UInt32)(b & 0xFF))<<8)  |
                  (((UInt32)(c & 0xFF))<<16) |
                  (((UInt32)(d & 0xFF))<<24) ;
    return addr;
  }
  
} __attribute__ ((packed)); //__attribute__ ((aligned(1), packed));

} //namespace ipstack

#endif // __IPV4__ 

