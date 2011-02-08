#ifndef __IPSTACK_CONFIG_H__
#define __IPSTACK_CONFIG_H__

namespace ipstack{

//This specifies the maximum amount of packets that
//can be buffered for each connection. This affects
//TCP sending and receiving and UDP receiving.
//A power of 2 is most efficient.
#define __IPSTACK_MAX_PACKETS__ 64

// *** Default mempool configuration
enum {
  BLOCKSIZE_BIG = 1514,
  COUNT_BIG = 64, //a power of 2 is most efficient
  BLOCKSIZE_SMALL = 128,
  COUNT_SMALL = 32 //a power of 2 is most efficient
};

} //namespace ipstack

#endif // __IPSTACK_CONFIG_H__

