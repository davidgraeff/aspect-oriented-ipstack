#include "../TCP_Socket.h"

namespace ipstack {
  
void TCP_Socket::closed(TCP_Segment* segment, unsigned len) {
  if(segment != 0){
    // new tcp segment received:
    //printf("ERROR: TCP Segment in CLOSED state arrived\n");
    //printf("-> forgot to 'unbind()' ?\n");
    free(segment);
  }
  else{
    // there are no more segments in the input buffer
    // nothing to do here
  }
  waiting = false; // no more segments to wait for
}

} //namespace ipstack
