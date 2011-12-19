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


#ifndef __ETH_FRAME__
#define __ETH_FRAME__

#include "util/types.h"
#include <string.h> //for memcpy

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
    memcpy(dst_hwaddr, (UInt8*)dst, 6);
  }
  
  UInt8* get_dst_hwadd() { return dst_hwaddr; }
  
  void set_dst_hwaddr_broadcast(){
    memset(dst_hwaddr, 0xFF, 6);
  }
  
  void set_src_hwaddr(const UInt8* src){
    memcpy(src_hwaddr, (UInt8*)src, 6);
  }
  
  UInt8* get_src_hwaddr() { return src_hwaddr; }
  
} __attribute__ ((packed));

} // namespace ipstack

#endif /* __ETH_FRAME__ */
