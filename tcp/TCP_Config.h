#ifndef __TCP_CONFIG_H__
#define __TCP_CONFIG_H__

#include "../IPStack_Config.h"

//This is the maximum amount of packets that can be stored in the
//tcp receivebuffer foreach connection
#ifndef __TCP_RECEIVEBUFFER_MAX_PACKETS__
  #define __TCP_RECEIVEBUFFER_MAX_PACKETS__ __IPSTACK_MAX_PACKETS__
  //#define __TCP_RECEIVEBUFFER_MAX_PACKETS__ 10 //user customization possible
  //set this value to 1 if you don't use a sliding receive window
  //and if you don't use pure-variants
#endif

//This is the maximum amount of packets that can be send simultaneously
//foreach tcp connection
#ifndef __TCP_HISTORY_MAX_PACKETS__
  #define __TCP_HISTORY_MAX_PACKETS__ __IPSTACK_MAX_PACKETS__
  //#define __TCP_HISTORY_MAX_PACKETS__ 5 //user customization possible
  //set this value to 1 if you don't use a sliding send window
  //and if you don't use pure-variants
#endif

#endif // __TCP_CONFIG_H__

