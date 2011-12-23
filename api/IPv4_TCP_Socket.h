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
#include "../util/Ringbuffer.h"
#include "../IPStack_Config.h"

namespace IP {

//static template metaprogram assertions

//check if the buffer count is not 0
template<unsigned BUFFER_COUNT>
class TCP_Buffer_Count_Assertion{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0; };
template<>class TCP_Buffer_Count_Assertion<0>{};

//check if the buffer size is a least 44 bytes
template<unsigned BUFFER_SIZE, bool OK=(BUFFER_SIZE >= 44)>
class TCP_Buffer_Size_Assertion_44{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Size_of_big_buffer_must_be_at_least_44_bytes; };
template<unsigned BUFFER_SIZE>class TCP_Buffer_Size_Assertion_44<BUFFER_SIZE, false>{};

//check if the buffer size is a least 40 bytes
template<unsigned BUFFER_SIZE, bool OK=(BUFFER_SIZE >= 40)>
class TCP_Buffer_Size_Assertion_40{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Size_of_small_buffer_must_be_at_least_40_bytes; };
template<unsigned BUFFER_SIZE>class TCP_Buffer_Size_Assertion_40<BUFFER_SIZE, false>{};


//the actual API
template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG,
         unsigned tCOUNT_1 = ipstack::COUNT_BIG,
         unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL,
         unsigned tCOUNT_2 = ipstack::COUNT_SMALL>
class IPv4_TCP_Socket : public ipstack::IPv4_TCP_Socket {
  private:
  
  //create the type for the following mempool instantiation
  typedef typename ipstack::MempoolAPI<tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2, ipstack::MEMORY_GENERIC>::Type TCP_Mempool;
 
  //static assertion: check whether at least 2 buffers (a big and small one) are available for this TCP Socket. If not, throw a compile-time error.
  typedef typename TCP_Buffer_Count_Assertion<TCP_Mempool::COUNT_BIG>::CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0 big_buf_cnt_assert;
  typedef typename TCP_Buffer_Count_Assertion<TCP_Mempool::COUNT_SMALL>::CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0 small_buf_cnt_assert;
  
  //static assertion: check whether the size of each buffer is at least 40(44) bytes (IP header + TCP header (+ TCP MSS Option))
  typedef typename TCP_Buffer_Size_Assertion_44<TCP_Mempool::SIZE_BIG>::CONFIGURATION_ERROR_FOR_TCP__Size_of_big_buffer_must_be_at_least_44_bytes big_buf_size_assert;
  typedef typename TCP_Buffer_Size_Assertion_40<TCP_Mempool::SIZE_SMALL>::CONFIGURATION_ERROR_FOR_TCP__Size_of_small_buffer_must_be_at_least_40_bytes small_buf_size_assert;

  //the mempool attribute
  TCP_Mempool pool;
  
  //size of ringbuffer: tCOUNT_1+tCOUNT_2 (if generic api is enabled, else PACKET_LIMIT)
  typename ipstack::PacketbufferAPI<tCOUNT_1+tCOUNT_2, ipstack::MEMORY_GENERIC>::Type buf;

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

} // namespace IP

#endif // __IPSTACK_API_IPV4_TCP_SOCKET_H__
