#ifndef __INTERNET_CHECKSUM__
#define __INTERNET_CHECKSUM__

#include "IPv4.h"
#include "../router/Interface.h"
#include "util/types.h"

namespace ipstack {

class InternetChecksum {
  private:
  static inline UInt16 byteswap16(UInt16 val) {
    return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
  }

  static inline UInt16 invert(UInt32 csum, Interface* interface){
    return (~csum & 0xFFFF);
  }

  public:
  static UInt32 computePseudoHeader(IPv4_Packet* packet, UInt16 len){
    UInt32 csum = len; //length of (udp, tcp) frame
  
    csum += packet->get_protocol(); // protocol
  
    UInt32 src = packet->get_src_ipaddr();
    csum += byteswap16(src); // source address (1)
    csum += byteswap16(src>>16); // source address (2)

    UInt32 dst = packet->get_dst_ipaddr();
    csum += byteswap16(dst); // destination address (1)
    csum += byteswap16(dst>>16); // destination address (2)
  
    return csum;
  }

  static UInt32 computePayload(IPv4_Packet* packet, UInt16 len, Interface* interface){
    //Software Checksumming
    UInt32 csum = 0;
  
    if(len & 0x1){
      //odd datasize: add padding byte (zero)
      UInt16 odd = packet->get_data()[len-1];
      csum += (odd & 0xFF) << 8;
    }

    len /= 2;

    UInt16* payload = (UInt16*) packet->get_data();
    for(unsigned i=0; i<len; ++i){
      csum += byteswap16(*payload++);
    }
    return csum;
  }

  static UInt16 compute(IPv4_Packet* packet, Interface* interface){
    UInt16 len = packet->get_total_len() - (packet->get_ihl() * 4);

    UInt32 csum = computePseudoHeader(packet, len);
  
    csum += computePayload(packet, len, interface);

    while (csum >> 16) {
      csum = (csum & 0xFFFF) + (csum >> 16); // accumulate carry bits
    }
  
    return invert(csum, interface); // one's complement
  }
  
  static UInt16 computePayloadChecksum(IPv4_Packet* packet, Interface* interface){
    UInt16 len = packet->get_total_len() - (packet->get_ihl() * 4);

    UInt32 csum = computePayload(packet, len, interface);

    while (csum >> 16) {
      csum = (csum & 0xFFFF) + (csum >> 16); // accumulate carry bits
    }
  
    return invert(csum, interface); // one's complement
  }

  static bool valid(IPv4_Packet* packet, Interface* interface){
    return (compute(packet, 0) == 0);
  }

};

} //namespace ipstack

#endif // __INTERNET_CHECKSUM__ 

