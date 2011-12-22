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


#ifndef __IPSTACK_API_IPV4_TCP_SOCKET_H__
#define __IPSTACK_API_IPV4_TCP_SOCKET_H__

#include "../ipv4/ipv4_tcp/IPv4_TCP_Socket.h"
#include "../util/Mempool.h"
#include "../util/MempoolConfig.h"
#include "../util/Ringbuffer.h"
#include "../util/RingbufferConfig.h"
#include "../IPStack_Config.h"

namespace ipstack {
namespace api {

//static template metaprogram assertion
template<unsigned BUFFER_COUNT>
class TCP_Buffer_Assertion{ public: typedef void CONFIGURATION_ERROR__TCP_requires_at_least_2_buffers; };
template<>class TCP_Buffer_Assertion<0>{};
template<>class TCP_Buffer_Assertion<1>{};


//the actual API
template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG,
         unsigned tCOUNT_1 = ipstack::COUNT_BIG,
         unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL,
         unsigned tCOUNT_2 = ipstack::COUNT_SMALL,
         unsigned tRINGBUFFERSIZE = __IPSTACK_MAX_PACKETS__>
class IPv4_TCP_Socket : public ipstack::IPv4_TCP_Socket {
  private:
  
  //create the type for the following mempool instantiation
  typedef typename MempoolAPI<tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2, __IPSTACK_GENERIC_MEMPOOL__>::Type TCP_Mempool;
 
  //static assertion: check whether at least 2 buffer are available for this TCP Socket. If not, throw a compile-time error.
  typedef typename TCP_Buffer_Assertion<TCP_Mempool::COUNT_BIG + TCP_Mempool::COUNT_SMALL>::CONFIGURATION_ERROR__TCP_requires_at_least_2_buffers buf_assert;

  //the mempool attribute
  TCP_Mempool pool;
  
  //if tRINGBUFFERSIZE is specified (= no default paramater), use it.
  //else choose max{ __IPSTACK_MAX_PACKETS__, tCOUNT_1+tCOUNT_2 }
  enum { RING_BUF_SIZE = (tRINGBUFFERSIZE != __IPSTACK_MAX_PACKETS__) ?
                          tRINGBUFFERSIZE :
                         (__IPSTACK_MAX_PACKETS__ >= tCOUNT_1+tCOUNT_2) ?
                          __IPSTACK_MAX_PACKETS__ : tCOUNT_1+tCOUNT_2 };
  
  typename PacketbufferAPI<RING_BUF_SIZE, __IPSTACK_GENERIC_RINGBUFFER__>::Type buf;

  public:
  IPv4_TCP_Socket(){
    set_Mempool(&pool);
    setMaxMTU(TCP_Mempool::SIZE_BIG);
    setMaxReceiveWindow(TCP_Mempool::COUNT_BIG);
    set_packetbuffer(&buf);
  }

  //for placement new: calling the constructor explicitly for global objects
  void* operator new(__SIZE_TYPE__ size, void* mem) {
    return mem;
  }
};

}} // namespace ipstack::api

#endif // __IPSTACK_API_IPV4_TCP_SOCKET_H__
