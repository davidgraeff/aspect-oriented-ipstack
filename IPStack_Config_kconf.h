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


#ifndef __IPSTACK_CONFIG_H__
#define __IPSTACK_CONFIG_H__

#include "ciaoConfig.h"

namespace ipstack{

//This specifies the maximum amount of packets that
//can be buffered for each connection. This affects
//TCP sending and receiving and UDP receiving.
//A power of 2 is most efficient.
#ifdef cfIPSTACK_MAX_PACKETS
  #define __IPSTACK_MAX_PACKETS__ cfIPSTACK_MAX_PACKETS
#else
  #define __IPSTACK_MAX_PACKETS__ 64
#endif

// *** Default mempool configuration (from kconf)
enum {
  BLOCKSIZE_BIG = cfIPSTACK_BLOCKSIZE_BIG,
  COUNT_BIG = cfIPSTACK_COUNT_BIG,
  BLOCKSIZE_SMALL = cfIPSTACK_BLOCKSIZE_SMALL,
  COUNT_SMALL = cfIPSTACK_COUNT_SMALL
};

// Max number of TCP retransmissions set by kconfig (default 10)
#ifdef cfTCP_MAX_RETRANSMISSIONS
  #define __TCP_MAX_RETRANSMISSIONS__ cfTCP_MAX_RETRANSMISSIONS
#else
  #define __TCP_MAX_RETRANSMISSIONS__ 10
#endif

} //namespace ipstack

#endif // __IPSTACK_CONFIG_H__

