#ifndef __ICMP__
#define __ICMP__

#include "util/types.h"

namespace ipstack {

class ICMP_Packet{
  public:
  enum { ICMP_HEADER_SIZE = 8,
         IPV4_TYPE_ICMP = 1,
         ICMP_TYPE_DESTINATION_UNREACHABLE = 3,
         ICMP_CODE_PROTOCOL_UNREACHABLE = 2,
         ICMP_CODE_PORT_UNREACHABLE = 3,
         ICMP_TYPE_ECHO_REQUEST = 8,
         ICMP_TYPE_ECHO_REPLY = 0,
         ICMP_CODE_ECHO = 0 };

  private:
  UInt8 type;
  UInt8 code;
  UInt16 checksum;
  UInt32 quench;
  UInt8 data[];
  
  public:
  UInt8 get_type() { return type; }
  void set_type(UInt8 t) { type = t; }
  
  UInt8 get_code() { return code; }
  void set_code(UInt8 c) { code = c; }
  
  UInt16 get_checksum() { return checksum; }
  void set_checksum(UInt16 csum) { checksum = csum; }
  
  UInt32 get_quench() { return quench; }
  void set_quench(UInt32 q) { quench = q; }
  
  UInt8* get_data() { return data; }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif // __ICMP__
