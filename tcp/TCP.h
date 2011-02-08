#ifndef __TCP__
#define __TCP__

#include "util/types.h"

namespace ipstack {

class TCP_Segment{
  public:
  enum { TCP_MIN_HEADER_SIZE = 20,
         IPV4_TYPE_TCP = 0x6,
         UNUSED_PORT = 0, // http://www.iana.org/assignments/port-numbers
         DEFAULT_MSS = 536 }; //rfc879
         
  enum { FIN_FLAG = 1 << 0,
         SYN_FLAG = 1 << 1,
         RST_FLAG = 1 << 2,
         PSH_FLAG = 1 << 3,
         ACK_FLAG = 1 << 4,
         URG_FLAG = 1 << 5 };
  
  private:
  UInt16 sport;
  UInt16 dport;
  UInt32 seqnum;
  UInt32 acknum;
  UInt8 header_len; //upper 4 bits, in 32bit words
  UInt8 flags; //lower 6 bits
  UInt16 window;
  UInt16 checksum;
  UInt16 urgend_ptr;
  UInt8 data[];
  
  public:
  UInt16 get_sport() { return sport; }
  void set_sport(UInt16 s) { sport = s; }
  
  UInt16 get_dport() { return dport; }
  void set_dport(UInt16 d) { dport = d; }
  
  UInt32 get_seqnum() { return seqnum; }
  void set_seqnum(UInt32 s) { seqnum = s; }
  
  UInt32 get_acknum() { return acknum; }
  void set_acknum(UInt32 a) { acknum = a; }
  
  UInt8 get_header_len() { return ((header_len >> 4) & 0xF); }
  void set_header_len(UInt8 len) { header_len = ((len << 4) & 0xF0); }
  
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
  
  void set_flags(UInt8 f){ flags = f; }
  
  UInt16 get_window() { return window; }
  void set_window(UInt16 w) { window = w; }
  
  UInt16 get_checksum() { return checksum; }
  void set_checksum(UInt16 c) { checksum = c; }
  
  UInt16 get_urgend_ptr() { return urgend_ptr; }
  void set_urgend_ptr(UInt16 up) { urgend_ptr = up; }
  
  UInt8* get_data() {
    //remove tcp header + tcp options
    //do not return data[] directly because of variable header length
    return (UInt8*) (((UInt32*) this) + get_header_len());
  }
  
  UInt8 get_options_len() { return (get_header_len() - TCP_MIN_HEADER_SIZE/4)*4; }
  UInt8* get_options() { return data; }
  
  //TCP Illustrated Vol. 2 page 810
  static inline bool SEQ_LT(UInt32 a, UInt32 b)  { return ((Int32)(a-b) < 0 ); }
  static inline bool SEQ_LEQ(UInt32 a, UInt32 b) { return ((Int32)(a-b) <= 0); }
  static inline bool SEQ_GT(UInt32 a, UInt32 b)  { return ((Int32)(a-b) > 0 ); }
  static inline bool SEQ_GEQ(UInt32 a, UInt32 b) { return ((Int32)(a-b) >= 0); }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif /* __TCP__ */

