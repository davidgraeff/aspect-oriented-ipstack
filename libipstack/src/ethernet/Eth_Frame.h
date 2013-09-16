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
#include <string.h> //for memcpy

namespace ipstack {

class Eth_Frame {
  public:
  enum { ETH_HEADER_SIZE = 14U,
         MAX_FRAME_SIZE = 1518U };
  
  private:
  uint8_t dst_hwaddr[6];
  uint8_t src_hwaddr[6];
  uint16_t type; //big endian
  uint8_t data[]; //max 1500
  
  public:
  uint16_t get_type(){ return type; }
  void set_type(uint16_t t) { type = t; }
  uint8_t* get_data() { return data; }
  
  void set_dst_hwaddr(const uint8_t* dst){
    memcpy(dst_hwaddr, (uint8_t*)dst, 6);
  }
  
  uint8_t* get_dst_hwaddr() { return dst_hwaddr; }
  
  void set_dst_hwaddr_broadcast(){
	  dst_hwaddr[0] = 0xff;
	  dst_hwaddr[1] = 0xff;
	  dst_hwaddr[2] = 0xff;
	  dst_hwaddr[3] = 0xff;
	  dst_hwaddr[4] = 0xff;
	  dst_hwaddr[5] = 0xff;
  }
  
  void set_src_hwaddr(const uint8_t* src){
	  src_hwaddr[0] = src[0];
	  src_hwaddr[1] = src[1];
	  src_hwaddr[2] = src[2];
	  src_hwaddr[3] = src[3];
	  src_hwaddr[4] = src[4];
	  src_hwaddr[5] = src[5];
	  //memcpy(src_hwaddr, (uint8_t*)src, 6);
  }
  
  uint8_t* get_src_hwaddr() { return src_hwaddr; }
  
} __attribute__ ((packed));

} // namespace ipstack
