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

#include "MemoryInterface.h"
#include "Mempool_Config.h"

namespace ipstack
{

template<unsigned BLOCKSIZE, unsigned COUNT>
class SingleMempool : public MemoryInterface
{
	private:
		char pool[BLOCKSIZE* COUNT];
		char* freelist[COUNT];
		char** head;

	public:
		SingleMempool() : head(freelist + COUNT - 1) {
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
};


template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG, unsigned tCOUNT_1=ipstack::COUNT_BIG,
unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL, unsigned tCOUNT_2 = ipstack::COUNT_SMALL>
class BasicMempool : public MemoryInterface
{
		//This class is exactly as efficient as "BasicMempool" if tCOUNT_2 = 0
	public:
		enum { SIZE_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
			   SIZE_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
			   COUNT_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2,
			   COUNT_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2
			 };

	private:
		SingleMempool<SIZE_BIG, COUNT_BIG> mempool_big;
		SingleMempool<SIZE_SMALL, COUNT_SMALL> mempool_small;

	public:

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
};

} //namespace ipstack
