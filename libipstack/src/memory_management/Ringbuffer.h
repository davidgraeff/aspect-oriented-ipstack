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
#include "util/ipstack_inttypes.h"
#include "Mempool_Config.h"
#include "memory_management/MemoryInterface.h"

namespace ipstack {

template<typename tIndexType, unsigned tBUFFERSIZE>
class BasicRingbuffer : public RingbufferInterface {

	enum { BUFFERSIZE};

	void* buffer[BUFFERSIZE];

	tIndexType inpos_;
	tIndexType outpos_;

public:
	BasicRingbuffer() : inpos_(0), outpos_(0) {}

	void put(void* val) volatile __attribute__ ((noinline)) {
		if (((inpos_ + 1) % BUFFERSIZE) != outpos_) {
			buffer[inpos_] = val;
			inpos_ = (inpos_ + 1) % BUFFERSIZE;
		} else {
			// buffer full!
		}
	}

	void* get() volatile __attribute__ ((noinline)) {
		if (outpos_ != inpos_) {
			void* val = buffer[outpos_];
			outpos_ = (outpos_ + 1) % BUFFERSIZE;

			return val;
		} else {
			return 0;
		}
	}
	
	bool isEmpty() volatile const __attribute__ ((noinline)) {
		return outpos_ == inpos_ ? true : false;
	}

	bool isFull() volatile const __attribute__ ((noinline)) {
		return ((inpos_ + 1) % BUFFERSIZE) != outpos_ ? false : true;
	}
};


template<>
class RingbufferType {
  public:
  typedef BasicRingbuffer<PACKET_LIMIT> Packetbuffer;
};

// The 'Packetbuffer' type used everywhere
typedef RingbufferType::Packetbuffer Packetbuffer;


// only used for the API
template<uint8_t tBUFFERSIZE, unsigned tGENERIC=0>
class PacketbufferAPI {
  public:
  typedef Packetbuffer Type;
};

template<uint8_t tBUFFERSIZE>
class PacketbufferAPI<tBUFFERSIZE, 1> {
  public:
  typedef BasicRingbuffer<tBUFFERSIZE> Type;
};

} //namespace ipstack


