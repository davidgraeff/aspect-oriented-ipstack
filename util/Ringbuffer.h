#ifndef __IPSTACK_RINGBUFFER_H__
#define __IPSTACK_RINGBUFFER_H__

#include "util/types.h"

#include "RingbufferBase.h"
#include "RingbufferConfig.h"
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
  typedef BasicRingbuffer<EmptyRingbufferBase, __IPSTACK_MAX_PACKETS__> Packetbuffer;
};

template<> // template specialization for '1'
class RingbufferType<1> {
  public:
  typedef PolymorphRingbufferBase Packetbuffer;
};

// The 'Packetbuffer' type used everywhere
typedef RingbufferType<__IPSTACK_GENERIC_RINGBUFFER__>::Packetbuffer Packetbuffer;


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

