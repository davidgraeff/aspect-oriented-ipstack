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


#ifndef __IPSTACK_RINGBUFFER_H__
#define __IPSTACK_RINGBUFFER_H__

#include "util/types.h"

#include "RingbufferBase.h"
#include "../IPStack_Config.h"

namespace ipstack {

template<typename tBASE, unsigned tBUFFERSIZE>
class BasicRingbuffer : public tBASE {

	// limit actual buffer size to [2...255] (8 bit)
	enum { BUFFERSIZE = (tBUFFERSIZE < 2) ? 2 :
	                    ((tBUFFERSIZE > 255) ? 255 : tBUFFERSIZE) };

	void* buffer[BUFFERSIZE];

	UInt8 inpos_;
	UInt8 outpos_;

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

	/*bool isEmpty() volatile const {
		return outpos_ == inpos_ ? true : false;
	}*/

	bool isFull() volatile const __attribute__ ((noinline)) {
		return ((inpos_ + 1) % BUFFERSIZE) != outpos_ ? false : true;
	}

};


template<unsigned tGENERIC=0>
class RingbufferType {
  public:
  typedef BasicRingbuffer<EmptyRingbufferBase, PACKET_LIMIT> Packetbuffer;
};

template<> // template specialization for '1'
class RingbufferType<1> {
  public:
  typedef PolymorphRingbufferBase Packetbuffer;
};

// The 'Packetbuffer' type used everywhere
typedef RingbufferType<MEMORY_GENERIC>::Packetbuffer Packetbuffer;


// only used for the API
template<UInt8 tBUFFERSIZE, unsigned tGENERIC=0>
class PacketbufferAPI {
  public:
  typedef Packetbuffer Type;
};

template<UInt8 tBUFFERSIZE>
class PacketbufferAPI<tBUFFERSIZE, 1> {
  public:
  typedef BasicRingbuffer<PolymorphRingbufferBase, tBUFFERSIZE> Type;
};

} //namespace ipstack


#endif // __IPSTACK_RINGBUFFER_H__

