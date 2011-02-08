#ifndef __MEMPOOL_BASE_H__
#define __MEMPOOL_BASE_H__

//namespace ipstack {

class PolymorphMempoolBase {
public:
  virtual void* alloc(unsigned size) = 0;
  virtual void free(void* chunk) = 0;
};

class EmptyMempoolBase {};

//} //namespace ipstack

#endif /* __MEMPOOL_BASE_H__ */

