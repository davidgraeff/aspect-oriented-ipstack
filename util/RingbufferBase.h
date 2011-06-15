#ifndef __RINGBUFFER_BASE_H__
#define __RINGBUFFER_BASE_H__

namespace ipstack {

class PolymorphRingbufferBase {
public:
  virtual void put(void* val) volatile = 0;
  virtual void* get() volatile = 0;
  virtual bool isFull() volatile const = 0;
};

class EmptyRingbufferBase {};

} //namespace ipstack

#endif // __RINGBUFFER_BASE_H__


