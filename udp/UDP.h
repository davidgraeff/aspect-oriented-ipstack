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


#ifndef __UDP__
#define __UDP__

#include "util/types.h"

namespace ipstack {

class UDP_Packet{
  public:
  enum { UDP_HEADER_SIZE = 8,
         IPV4_TYPE_UDP = 17,
         UNUSED_PORT = 0 }; // http://www.iana.org/assignments/port-numbers

  private:
  UInt16 sport;
  UInt16 dport;
  UInt16 length;
  UInt16 checksum;
  UInt8 data[];
  
  public:
  UInt16 get_sport() { return sport; }
  void set_sport(UInt16 s) { sport = s; }
  
  UInt16 get_dport() { return dport; }
  void set_dport(UInt16 d) { dport = d; }
  
  UInt16 get_length() { return length; }
  void set_length(UInt16 len) { length = len; }
  
  UInt16 get_checksum() { return checksum; }
  void set_checksum(UInt16 csum) { checksum = csum; }
  
  UInt8* get_data() { return data; }
  
} __attribute__ ((packed));

} //namespace ipstack

#endif // __UDP__
