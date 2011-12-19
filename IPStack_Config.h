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

namespace ipstack{

//This specifies the maximum amount of packets that
//can be buffered for each connection. This affects
//TCP sending and receiving and UDP receiving.
//A power of 2 is most efficient.
#define __IPSTACK_MAX_PACKETS__ 64

// *** Default mempool configuration
enum {
  BLOCKSIZE_BIG = 1514,
  COUNT_BIG = 64, //a power of 2 is most efficient
  BLOCKSIZE_SMALL = 128,
  COUNT_SMALL = 32 //a power of 2 is most efficient
};

} //namespace ipstack

#endif // __IPSTACK_CONFIG_H__

