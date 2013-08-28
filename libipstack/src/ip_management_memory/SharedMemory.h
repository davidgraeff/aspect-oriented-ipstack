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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "cfAttribs.h"
#include "../util/Mempool.h"
#include "util/Ringbuffer.h"

#include <string.h>
#include <inttypes.h>
namespace ipstack
{
class SendBuffer;
//static template metaprogram assertions

//check if the buffer count is not 0
template<unsigned BUFFER_COUNT>
class SharedMemory_Count_Assertion{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0; };
template<>class SharedMemory_Count_Assertion<0>{};

//check if the buffer size is a least 44 bytes
template<unsigned BUFFER_SIZE, bool OK=(BUFFER_SIZE >= 44)>
class SharedMemory_Size_Assertion_44{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Size_of_big_buffer_must_be_at_least_44_bytes; };
template<unsigned BUFFER_SIZE>class SharedMemory_Size_Assertion_44<BUFFER_SIZE, false>{};

//check if the buffer size is a least 40 bytes
template<unsigned BUFFER_SIZE, bool OK=(BUFFER_SIZE >= 40)>
class SharedMemory_Size_Assertion_40{ public: typedef void CONFIGURATION_ERROR_FOR_TCP__Size_of_small_buffer_must_be_at_least_40_bytes; };
template<unsigned BUFFER_SIZE>class SharedMemory_Size_Assertion_40<BUFFER_SIZE, false>{};

class SharedMemory {
	public:
	
	//create the type for the following mempool instantiation
	typedef ipstack::MempoolAPI<cfIPSTACK_MANAGEMENT_BLOCKSIZE_BIG, cfIPSTACK_MANAGEMENT_COUNT_BIG, cfIPSTACK_MANAGEMENT_BLOCKSIZE_SMALL, cfIPSTACK_MANAGEMENT_COUNT_SMALL, ipstack::MEMORY_GENERIC>::Type SharedMemory_Mempool;
	
	//static assertion: check whether at least 2 buffers (a big and small one) are available for this TCP Socket. If not, throw a compile-time error.
	typedef SharedMemory_Count_Assertion<SharedMemory_Mempool::COUNT_BIG>::CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0 big_buf_cnt_assert;
	typedef SharedMemory_Count_Assertion<SharedMemory_Mempool::COUNT_SMALL>::CONFIGURATION_ERROR_FOR_TCP__Count_of_buffer_must_not_be_0 small_buf_cnt_assert;
	
	//static assertion: check whether the size of each buffer is at least 40(44) bytes (IP header + TCP header (+ TCP MSS Option))
	typedef SharedMemory_Size_Assertion_44<SharedMemory_Mempool::SIZE_BIG>::CONFIGURATION_ERROR_FOR_TCP__Size_of_big_buffer_must_be_at_least_44_bytes big_buf_size_assert;
	typedef SharedMemory_Size_Assertion_40<SharedMemory_Mempool::SIZE_SMALL>::CONFIGURATION_ERROR_FOR_TCP__Size_of_small_buffer_must_be_at_least_40_bytes small_buf_size_assert;

	// the mempool attribute. Because mempool provide no functionality to iterate it, we have to 
	// keep pointers to the allocated objects
	SharedMemory_Mempool mempool;
	
	/**
		* Return amount of memory blocks available.
		*/
	enum {SLOTS = cfIPSTACK_MANAGEMENT_COUNT_BIG + cfIPSTACK_MANAGEMENT_COUNT_SMALL};

	SendBuffer* allocatedMemories[SLOTS];
	/**
	  * Insert a SendBuffer pointer into the management list
	  */
	bool insert(SendBuffer* data);
	/**
	  * Remove a SendBuffer pointer from the management list identified by the list number.
	  */
	bool remove(uint_fast16_t index);
	/**
	  * Use this to iterate over all used list entries. "index" will be modified
	  * to point to the next found list number where the returned Sendbuffer has been
	  * found. Return 0 if no valid entry could be found. The search start with the given
	  * index.
	  */
	SendBuffer* getNext(uint_fast16_t& index) const;
	/**
	  * Check for all allocated SendBuffers if the data has been send already.
	  * If this is the case, free the memory block / Sendbuffer.
	  * This method is called internally by every call to @allocSendBuffer.
	  */
	void freeAllUnused();

	SharedMemory();

	//for placement new: calling the constructor explicitly for global objects
	void* operator new(__SIZE_TYPE__ size, void* mem) {
		return mem;
	}
};

/**
  * SharedMemory is actually a singleton. Use this "sharedmemory" variable.
  */
extern SharedMemory sharedmemory;

} //namespace ipstack
