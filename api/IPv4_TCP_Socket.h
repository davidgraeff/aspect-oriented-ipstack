#ifndef __IPSTACK_API_IPV4_TCP_SOCKET_H__
#define __IPSTACK_API_IPV4_TCP_SOCKET_H__

#include "../ipv4/ipv4_tcp/IPv4_TCP_Socket.h"
#include "../util/Mempool.h"
#include "../util/MempoolConfig.h"
#include "../util/Ringbuffer.h"
#include "../util/RingbufferConfig.h"
#include "../IPStack_Config.h"

namespace ipstack {
namespace api {

template<unsigned tBLOCKSIZE_1 = ipstack::BLOCKSIZE_BIG,
         unsigned tCOUNT_1 = ipstack::COUNT_BIG,
         unsigned tBLOCKSIZE_2 = ipstack::BLOCKSIZE_SMALL,
         unsigned tCOUNT_2 = ipstack::COUNT_SMALL,
         unsigned tRINGBUFFERSIZE = __IPSTACK_MAX_PACKETS__>
class IPv4_TCP_Socket : public ipstack::IPv4_TCP_Socket {
  private:
  
  typename MempoolAPI<tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2, __IPSTACK_GENERIC_MEMPOOL__>::Type pool;
  
  //if tRINGBUFFERSIZE is specified (= no default paramater), use it.
  //else choose max{ __IPSTACK_MAX_PACKETS__, tCOUNT_1+tCOUNT_2 }
  enum { RING_BUF_SIZE = (tRINGBUFFERSIZE != __IPSTACK_MAX_PACKETS__) ?
                          tRINGBUFFERSIZE :
                         (__IPSTACK_MAX_PACKETS__ >= tCOUNT_1+tCOUNT_2) ?
                          __IPSTACK_MAX_PACKETS__ : tCOUNT_1+tCOUNT_2 };
  
  typename PacketbufferAPI<RING_BUF_SIZE, __IPSTACK_GENERIC_RINGBUFFER__>::Type buf;

  public:
  IPv4_TCP_Socket(){
    set_Mempool(&pool);
    setMaxMTU(pool.SIZE_BIG);
    setMaxReceiveWindow(pool.COUNT_BIG);
    set_packetbuffer(&buf);
  }

  //for placement new: calling the constructor explicitly for global objects
  void* operator new(__SIZE_TYPE__ size, void* mem) {
    return mem;
  }
};

}} // namespace ipstack::api

#endif // __IPSTACK_API_IPV4_TCP_SOCKET_H__
