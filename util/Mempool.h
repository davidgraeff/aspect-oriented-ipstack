#ifndef __IPSTACK_MEMPOOL_H__
#define __IPSTACK_MEMPOOL_H__

#include "MempoolBase.h"
#include "MempoolConfig.h"
#include "../IPStack_Config.h"

namespace ipstack {

template<unsigned BLOCKSIZE, unsigned COUNT>
class SingleMempool {
  private:
  char pool[BLOCKSIZE * COUNT];
  char* freelist[COUNT];
  char** head;
  
  public:
  SingleMempool() : head(freelist + COUNT - 1) {
    for(unsigned i=0; i < COUNT; ++i){
      freelist[i] = pool + i * BLOCKSIZE;
    }
  }
  
  void* alloc() {
    if(head < freelist){
      return 0;
    }
    return *head--;
  }

  bool free(void* mem) {
    if( (mem < pool) || (mem > pool + COUNT * BLOCKSIZE) ){
      return false;
    }
    
    //calculate the 'delta' to the next lower BLOCK
    unsigned rest = ((unsigned) (((char*)mem) - pool)) % BLOCKSIZE;
    
    *++head = ((char*) mem) - rest; //free at lower BLOCK boundary
    return true;
  }
};


template<typename tBASE, unsigned tBLOCKSIZE_1, unsigned tCOUNT_1, unsigned tBLOCKSIZE_2 = 256, unsigned tCOUNT_2 = 0>
class BasicMempool : public tBASE{
  //This class is exactly as efficient as "BasicMempool" if tCOUNT_2 = 0
  public:
  enum { SIZE_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
         SIZE_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tBLOCKSIZE_1 : tBLOCKSIZE_2,
         COUNT_BIG = tBLOCKSIZE_1 >= tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2,
         COUNT_SMALL = tBLOCKSIZE_1 < tBLOCKSIZE_2 ? tCOUNT_1 : tCOUNT_2 };
  
  private:
  SingleMempool<SIZE_BIG, COUNT_BIG> mempool_big;
  SingleMempool<SIZE_SMALL, COUNT_SMALL> mempool_small;
  
  public:
    
  void* alloc(unsigned size) __attribute__ ((noinline)) {
    if(size <= SIZE_SMALL){
      //try to use smaller mempool first
      void* ptr = mempool_small.alloc();
      if(ptr != 0){
        return ptr;
      }
    }
    if(size <= SIZE_BIG){
      return mempool_big.alloc();
    }
    return 0;
  }
  
  void free(void* chunk) __attribute__ ((noinline)) {
    if(mempool_small.free(chunk) == false){
      mempool_big.free(chunk);
    }
  }
  
};


// *** There are two versions of this Mempool available:
// *** ======================================================================================
// *** 1) A polymorphic version, which allows using different Mempool instances concurrently.
// ***    -> Run-time decision by virtual "alloc" and "free" functions. Increases binary size.
// ***
// *** 2) A static typed version, which allows only using one specific Mempool instance.
// ***    -> All types are known at compile time, no virtual function needed. Less overhead.

template<unsigned tGENERIC=0>
class MempoolType {
  public:
  typedef BasicMempool<EmptyMempoolBase,
                     ipstack::BLOCKSIZE_BIG,
                     ipstack::COUNT_BIG,
                     ipstack::BLOCKSIZE_SMALL,
                     ipstack::COUNT_SMALL> Mempool;
};

template<> // Template specialization for '1'
class MempoolType<1> {
  public:
  typedef PolymorphMempoolBase Mempool;
};

// The 'Mempool' type used everywhere
typedef MempoolType<__IPSTACK_GENERIC_MEMPOOL__>::Mempool Mempool;


// only used for the API
template<unsigned tBLOCKSIZE_1, unsigned tCOUNT_1, unsigned tBLOCKSIZE_2, unsigned tCOUNT_2, unsigned tGENERIC=0>
class MempoolAPI {
  public:
  typedef Mempool Type;
};

template<unsigned tBLOCKSIZE_1, unsigned tCOUNT_1, unsigned tBLOCKSIZE_2, unsigned tCOUNT_2>
class MempoolAPI<tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2, 1> {
  public:
  typedef BasicMempool<PolymorphMempoolBase, tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2> Type;
};

} //namespace ipstack

#endif /* __IPSTACK_MEMPOOL_H__ */

