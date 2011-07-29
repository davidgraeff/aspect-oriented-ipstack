#ifndef __IPSTACK_CONFIG_H__
#define __IPSTACK_CONFIG_H__

#include "ciaoConfig.h"

namespace ipstack{

//This specifies the maximum amount of packets that
//can be buffered for each connection. This affects
//TCP sending and receiving and UDP receiving.
//A power of 2 is most efficient.
#ifdef cfIPSTACK_MAX_PACKETS
  #define __IPSTACK_MAX_PACKETS__ cfIPSTACK_MAX_PACKETS
#else
  #define __IPSTACK_MAX_PACKETS__ 64
#endif

// *** Default mempool configuration (from kconf)
enum {
  BLOCKSIZE_BIG = cfIPSTACK_BLOCKSIZE_BIG,
  COUNT_BIG = cfIPSTACK_COUNT_BIG,
  BLOCKSIZE_SMALL = cfIPSTACK_BLOCKSIZE_SMALL,
  COUNT_SMALL = cfIPSTACK_COUNT_SMALL
};

// Max number of TCP retransmissions set by kconfig (default 10)
#ifdef cfTCP_MAX_RETRANSMISSIONS
  #define __TCP_MAX_RETRANSMISSIONS__ cfTCP_MAX_RETRANSMISSIONS
#else
  #define __TCP_MAX_RETRANSMISSIONS__ 10
#endif

} //namespace ipstack

#endif // __IPSTACK_CONFIG_H__

