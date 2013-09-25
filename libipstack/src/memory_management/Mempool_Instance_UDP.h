s// This file is part of Aspect-Oriented-IP.
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
// Copyright (C) 2011 Christoph Borchert, 2012-2013 David Gräff

#pragma once

#include "memory_management/SocketMemory.h"
#include "Mempool_Config.h"

namespace ipstack {

	//static template metaprogram assertions

	//check if the buffer count is not 0
	template<unsigned BUFFER_COUNT>
	class UDP_Buffer_Count_Assertion{ public: typedef void CONFIGURATION_ERROR_FOR_UDP__Count_of_buffer_must_not_be_0; };
	template<>class UDP_Buffer_Count_Assertion<0>{};

	//check if the buffer size is greater than 28 bytes
	template<unsigned BUFFER_SIZE, bool OK=(BUFFER_SIZE > 28)>
	class UDP_Buffer_Size_Assertion_28{ public: typedef void CONFIGURATION_ERROR_FOR_UDP__Size_of_buffer_must_be_greater_than_28_bytes; };
	template<unsigned BUFFER_SIZE>class UDP_Buffer_Size_Assertion_28<BUFFER_SIZE, false>{};


	//the actual API
	template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG,
			unsigned tCOUNT_1 = ipstack::COUNT_BIG,
			unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL,
			unsigned tCOUNT_2 = ipstack::COUNT_SMALL>
			
	class Mempool_Instance_UDP : public SocketMemory {
	private:
		
		//create the type for the following mempool instantiation
		typedef typename ipstack::Mempool<tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2> UDP_Mempool;
		
		//static assertion: check whether at least 1 buffer is available for this UDP Socket. If not, throw a compile-time error.
		typedef typename UDP_Buffer_Count_Assertion<UDP_Mempool::COUNT_BIG+UDP_Mempool::COUNT_SMALL>::CONFIGURATION_ERROR_FOR_UDP__Count_of_buffer_must_not_be_0 buf_cnt_assert;

		//static assertion: check whether the size of each buffer is greater that 28 bytes (IP header + UDP header)
		typedef typename UDP_Buffer_Size_Assertion_28<(UDP_Mempool::COUNT_BIG > 0) ? UDP_Mempool::SIZE_BIG : 29>::CONFIGURATION_ERROR_FOR_UDP__Size_of_buffer_must_be_greater_than_28_bytes big_buf_size_assert;
		typedef typename UDP_Buffer_Size_Assertion_28<(UDP_Mempool::COUNT_SMALL > 0) ? UDP_Mempool::SIZE_SMALL : 29>::CONFIGURATION_ERROR_FOR_UDP__Size_of_buffer_must_be_greater_than_28_bytes small_buf_size_assert;

		//the mempool attribute
		UDP_Mempool pool;
		
		//size of ringbuffer: tCOUNT_1+tCOUNT_2
		typename ipstack::PacketbufferAPI<tCOUNT_1+tCOUNT_2>::Type buf;
		
	public:
		Mempool_Instance_UDP() : SocketMemory(&pool, &buf) {}
	};

} // namespace IP
