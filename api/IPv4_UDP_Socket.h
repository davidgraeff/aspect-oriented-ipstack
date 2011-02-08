#ifndef __IPSTACK_API_IPV4_UDP_SOCKET_H__
#define __IPSTACK_API_IPV4_UDP_SOCKET_H__

#include "../ipv4/ipv4_udp/IPv4_UDP_Socket.h"
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
class IPv4_UDP_Socket : public ipstack::IPv4_UDP_Socket {
  private:
  
  #if __IPSTACK_GENERIC_MEMPOOL__
    BasicMempool<PolymorphMempoolBase, tBLOCKSIZE_1, tCOUNT_1, tBLOCKSIZE_2, tCOUNT_2> pool;
  #else
    Mempool pool;
  #endif // __IPSTACK_GENERIC_MEMPOOL__
  
  #if __IPSTACK_GENERIC_RINGBUFFER__
    //if tRINGBUFFERSIZE is specified (= no default paramater), use it.
    //else choose max{ __IPSTACK_MAX_PACKETS__, tCOUNT_1+tCOUNT_2 }
    enum { RING_BUF_SIZE = (tRINGBUFFERSIZE != __IPSTACK_MAX_PACKETS__) ?
                            tRINGBUFFERSIZE :
                           (__IPSTACK_MAX_PACKETS__ >= tCOUNT_1+tCOUNT_2) ?
                            __IPSTACK_MAX_PACKETS__ : tCOUNT_1+tCOUNT_2 };
    
    BasicRingbuffer<PolymorphRingbufferBase, RING_BUF_SIZE> buf;
  #else
    Packetbuffer buf;
  #endif // __IPSTACK_GENERIC_RINGBUFFER__
  
  public:
  IPv4_UDP_Socket(){
    set_Mempool(&pool);
    set_packetbuffer(&buf);
  }
};

}} // namespace ipstack::api

#endif // __IPSTACK_API_IPV4_UDP_SOCKET_H__
