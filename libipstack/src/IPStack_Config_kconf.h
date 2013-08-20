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

// *** Default mempool configuration (from kconf)
enum {
  BLOCKSIZE_BIG = cfIPSTACK_BLOCKSIZE_BIG,
  COUNT_BIG = cfIPSTACK_COUNT_BIG,
  BLOCKSIZE_SMALL = cfIPSTACK_BLOCKSIZE_SMALL,
  COUNT_SMALL = cfIPSTACK_COUNT_SMALL
};


#ifdef cfIPSTACK_MEMORY_GENERIC
  enum { MEMORY_GENERIC = 1,
         PACKET_LIMIT = cfIPSTACK_PACKET_LIMIT };
#else
  enum { MEMORY_GENERIC = 0,
         PACKET_LIMIT = COUNT_BIG+COUNT_SMALL };
#endif


#ifdef cfTCP_MAX_RETRANSMISSIONS
  enum { TCP_MAX_RETRANSMISSIONS = cfTCP_MAX_RETRANSMISSIONS };
#endif

} //namespace ipstack

#endif // __IPSTACK_CONFIG_H__

