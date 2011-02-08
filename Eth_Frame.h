#ifndef __ETH_FRAME__
#define __ETH_FRAME__

#include "util/types.h"

namespace ipstack {

class Eth_Frame {
  public:
  enum { ETH_HEADER_SIZE = 14U,
         MAX_FRAME_SIZE = 1518U };
  
  private:
  UInt8 dst_hwaddr[6];
  UInt8 src_hwaddr[6];
  UInt16 type; //big endian
  UInt8 data[]; //max 1500
  
  public:
  UInt16 get_type(){ return type; }
  void set_type(UInt16 t) { type = t; }
  UInt8* get_data() { return data; }
  
  void set_dst_hwaddr(const UInt8* dst){
    //use 32/16 bit integer arithmetic
    *((UInt32*) dst_hwaddr) = *((UInt32*) dst);
    *((UInt16*) (dst_hwaddr+4)) = *((UInt16*) (dst+4));
  }
  
  UInt8* get_dst_hwadd() { return dst_hwaddr; }
  
  void set_dst_hwaddr_broadcast(){
    //use 32/16 bit integer arithmetic
    *((UInt32*) dst_hwaddr) = 0xFFFFFFFFUL;
    *((UInt16*) (dst_hwaddr+4)) = 0xFFFFU;
  }
  
  void set_src_hwaddr(const UInt8* src){
    //use 32/16 bit integer arithmetic
    *((UInt32*) src_hwaddr) = *((UInt32*) src);
    *((UInt16*) (src_hwaddr+4)) = *((UInt16*) (src+4));
  }
  
  UInt8* get_src_hwaddr() { return src_hwaddr; }
  
} __attribute__ ((packed));

} // namespace ipstack

#endif /* __ETH_FRAME__ */
