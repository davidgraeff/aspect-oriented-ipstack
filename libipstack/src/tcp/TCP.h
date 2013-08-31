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


#ifndef __TCP__
#define __TCP__

#include "util/ipstack_inttypes.h"
#include "ip/InternetChecksum.h"

namespace ipstack {

class TCP_Segment{
  public:
  enum { TCP_MIN_HEADER_SIZE = 20,
         IP_TYPE_TCP = 0x6,
         UNUSED_PORT = 0, // http://www.iana.org/assignments/port-numbers
         DEFAULT_MSS = 536 }; //rfc879
         
  enum { FIN_FLAG = 1 << 0,
         SYN_FLAG = 1 << 1,
         RST_FLAG = 1 << 2,
         PSH_FLAG = 1 << 3,
         ACK_FLAG = 1 << 4,
         URG_FLAG = 1 << 5 };
  
  private:
  uint16_t sport;
  uint16_t dport;
  uint32_t seqnum;
  uint32_t acknum;
  uint8_t header_len; //upper 4 bits, in 32bit words
  uint8_t flags; //lower 6 bits
  uint16_t window;
  uint16_t checksum;
  uint16_t urgend_ptr;
  uint8_t data[];
  
  public:
  uint16_t get_sport() { return sport; }
  void set_sport(uint16_t s) { sport = s; }
  
  uint16_t get_dport() { return dport; }
  void set_dport(uint16_t d) { dport = d; }
  
  uint32_t get_seqnum() { return seqnum; }
  void set_seqnum(uint32_t s) { seqnum = s; }
  
  uint32_t get_acknum() { return acknum; }
  void set_acknum(uint32_t a) { acknum = a; }
  
  uint8_t get_header_len() { return ((header_len >> 4) & 0xF); }
  void set_header_len(uint8_t len) { header_len = ((len << 4) & 0xF0); }
  
  bool has_FIN() { return ((flags & FIN_FLAG) != 0); }
  bool has_SYN() { return ((flags & SYN_FLAG) != 0); }
  bool has_RST() { return ((flags & RST_FLAG) != 0); }
  bool has_PSH() { return ((flags & PSH_FLAG) != 0); }
  bool has_ACK() { return ((flags & ACK_FLAG) != 0); }
  bool has_URG() { return ((flags & URG_FLAG) != 0); }
  
  void set_FIN() { flags |= FIN_FLAG; }
  void set_SYN() { flags |= SYN_FLAG; }
  void set_RST() { flags |= RST_FLAG; }
  void set_PSH() { flags |= PSH_FLAG; }
  void set_ACK() { flags |= ACK_FLAG; }
  void set_URG() { flags |= URG_FLAG; }
  
  void set_flags(uint8_t f){ flags = f; }
  
  uint16_t get_window() { return window; }
  void set_window(uint16_t w) { window = w; }
  
  uint16_t get_checksum() { return checksum; }
  void set_checksum(uint16_t c) { checksum = c; }
	/**
	  * Calculate the tcp checksum.
	  * @param sum The pseudo header sum
	  * @param payloadlen We use extern information about the size of this packet
	  * @param The interface on which the packet will be send. Necessary for checksum offloading
	  */
	uint16_t calc_checksum(uint32_t csum, uint16_t payloadlen, Interface* interface) {
		csum += InternetChecksum::computePayload((uint8_t*)this, payloadlen);
		return ~InternetChecksum::accumulateCarryBits(csum); // one's complement
	}
	bool checksum_valid(uint32_t csum, uint16_t payloadlen, Interface* interface) {
		return calc_checksum(csum, payloadlen, interface) == 0;
	}
	
  uint16_t get_urgend_ptr() { return urgend_ptr; }
  void set_urgend_ptr(uint16_t up) { urgend_ptr = up; }
  
  uint8_t* get_data() {
    //remove tcp header + tcp options
    //do not return data[] directly because of variable header length
    return (uint8_t*) (((uint32_t*) this) + get_header_len());
  }
  
  uint8_t get_options_len() { return (get_header_len() - TCP_MIN_HEADER_SIZE/4)*4; }
  uint8_t* get_options() { return data; }
  
  //TCP Illustrated Vol. 2 page 810
  static inline bool SEQ_LT(uint32_t a, uint32_t b)  { return ((int32_t)(a-b) < 0 ); }
  static inline bool SEQ_LEQ(uint32_t a, uint32_t b) { return ((int32_t)(a-b) <= 0); }
  static inline bool SEQ_GT(uint32_t a, uint32_t b)  { return ((int32_t)(a-b) > 0 ); }
  static inline bool SEQ_GEQ(uint32_t a, uint32_t b) { return ((int32_t)(a-b) >= 0); }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif /* __TCP__ */

