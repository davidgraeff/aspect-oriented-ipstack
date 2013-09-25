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

/**
 * In this file you'll find a stack-based memory-pool, well suited for small
 * embedded systems. The first Mempool class provides one kind of size for
 * all slots and the second one provides two kinds of slots.
 * You propably not want to use those classes directly, but instead
 * the Mempool_instance_TCP or %_UDP for your TCP or UDP Socket. 
 */

#include "MemoryInterface.h"
#include "Mempool_Config.h"

namespace ipstack
{
	// forward decl
	template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG, unsigned tCOUNT_1=ipstack::COUNT_BIG,
	unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL, unsigned tCOUNT_2 = ipstack::COUNT_SMALL>
	class Mempool;
	
	/**
	 * This partial template specialization will be used if only one type of slots
	 * is required. Use it like this Mempool<10,10,0,0>. You have to set the last to
	 * template parameters to 0 otherwise the two-slot Mempool class will be used
	 * (because of the default template values we defined above).
	 * Warning: This implementation is not safe against double free's
	 */
	template<unsigned tBLOCKSIZE, unsigned tCOUNT>
	class Mempool<tBLOCKSIZE, tCOUNT,0,0> : public MemoryInterface
	{
		public:
			enum { BLOCKSIZE = tBLOCKSIZE, COUNT = tCOUNT};
			Mempool() : head(freelist + COUNT - 1) {
				for (unsigned i = 0; i < COUNT; ++i) {
					freelist[i] = pool + i * BLOCKSIZE;
				}
			}
	
			void* alloc() {
				if (head < freelist) {
					return 0;
				}
				return *(head--);
			}

			/**
				* Warning: This implementation is not safe against double free's
				*/
			bool free(void* mem) {
				if ((mem < pool) || (mem > pool + COUNT * BLOCKSIZE)) {
					return false;
				}
				
				//calculate the 'delta' to the next lower BLOCK
				unsigned rest = ((unsigned)(((char*)mem) - pool)) % BLOCKSIZE;

				*++head = ((char*) mem) - rest; //free at lower BLOCK boundary
				return true;
			}
			
			bool hasSpace() {
				return (head >= freelist);
			}
		private:
			char pool[BLOCKSIZE* COUNT];
			char* freelist[COUNT];
			char** head;
	};
	
	template<unsigned tBLOCKSIZE_1, unsigned tCOUNT_1, unsigned tBLOCKSIZE_2, unsigned tCOUNT_2>
	class Mempool : public MemoryInterface
	{
		public:
			enum { SIZE_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
				SIZE_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
				COUNT_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2,
				COUNT_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2
				};

			unsigned getMaxFreeBlockSize() {
				if (mempool_big.hasSpace())
					return SIZE_BIG;
				else if (mempool_small.hasSpace())
					return SIZE_SMALL;
				return 0;
			}
			
			void* alloc(unsigned size) __attribute__((noinline)) {
				if (size <= SIZE_SMALL) {
					//try to use smaller mempool first
					void* ptr = mempool_small.alloc();
					if (ptr != 0) {
						return ptr;
					}
				}
				if (size <= SIZE_BIG) {
					return mempool_big.alloc();
				}
				return 0;
			}

			void free(void* chunk) __attribute__((noinline)) {
				if (mempool_small.free(chunk) == false) {
					mempool_big.free(chunk);
				}
			}
			
		private:
			Mempool<SIZE_BIG, COUNT_BIG,0,0> mempool_big;
			Mempool<SIZE_SMALL, COUNT_SMALL,0,0> mempool_small;
	};

} //namespace ipstack