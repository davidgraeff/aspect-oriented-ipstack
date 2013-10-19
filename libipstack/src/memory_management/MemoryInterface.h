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
// Copyright (C) 2013 David Gräff

namespace ipstack {
	/**
	 * Interface for memory allocation and freeing.
	 */
	class MemoryInterface {
		public:
		virtual void* alloc(unsigned size) = 0;
		virtual void free(void* chunk) = 0;
		virtual unsigned getMaxFreeBlockSize() = 0;
	};

	class RingbufferInterface {
	public:
		virtual void put(void* val) = 0;
		virtual void* get() = 0;
		virtual bool isEmpty() = 0;
		virtual bool isFull() = 0;
	};
} //namespace ipstack

