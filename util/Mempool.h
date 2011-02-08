#ifndef __IPSTACK_MEMPOOL_H__
#define __IPSTACK_MEMPOOL_H__

#include "MempoolBase.h"
#include "MempoolConfig.h"
#include "../IPStack_Config.h"

//namespace ipstack {

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
// ***    -> Run-time decision by virtual "alloc" function. Adds 788 bytes to the binary.
// ***
// *** 2) A static typed version, which allows only using one specific Mempool instance.
// ***    -> All types are known at compile time, no virtual function needed. Less overhead.

#if __IPSTACK_GENERIC_MEMPOOL__
// 1) The polymorphic version:
typedef PolymorphMempoolBase Mempool;
typedef BasicMempool<PolymorphMempoolBase,
                     ipstack::BLOCKSIZE_BIG,
                     ipstack::COUNT_BIG,
                     ipstack::BLOCKSIZE_SMALL,
                     ipstack::COUNT_SMALL> SimpleMempool;

#else
// 2) The static typed version:
typedef BasicMempool<EmptyMempoolBase,
                     ipstack::BLOCKSIZE_BIG,
                     ipstack::COUNT_BIG,
                     ipstack::BLOCKSIZE_SMALL,
                     ipstack::COUNT_SMALL> Mempool;
typedef Mempool SimpleMempool;

#endif // __IPSTACK_GENERIC_MEMPOOL__

//} //namespace ipstack

#endif /* __IPSTACK_MEMPOOL_H__ */

