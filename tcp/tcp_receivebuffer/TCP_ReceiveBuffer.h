#ifndef __TCP_RECEIVEBUFFER_H__
#define __TCP_RECEIVEBUFFER_H__

#include "util/types.h"
#include "../TCP.h"
#include "../TCP_Config.h"
#include "TCP_RecvElement.h"
#include "../../util/Mempool.h"

#include <string.h>

namespace ipstack {

class TCP_Socket; //Forward declaration solves include cycle problem

class TCP_ReceiveBuffer{
  private:
  TCP_RecvElement elements[__TCP_RECEIVEBUFFER_MAX_PACKETS__];
  TCP_RecvElement* head;
  
  UInt32 read_firstSeqNum;
  bool pushFlag; //deliver all data to the application immediately
  //TODO: safe seqNum, until which data should be pushed
  TCP_Socket* socket; //for freeing packets

  public:
  TCP_ReceiveBuffer(TCP_Socket* sock) : head(0), pushFlag(false), socket(sock) {}
  
  void setFirstSeqNum(UInt32 seqNum) { read_firstSeqNum = seqNum; }
  
  bool isPushed() { return pushFlag; }
  
  unsigned getRecvBytes() {
    if(head == 0){
      return 0;
    }
    else{
      return head->getLength();
    }
  }

  UInt32 getAckNum();
  
  void socket_free(void* segment);
  
  void copyData(void* dst, unsigned len);
  
  //insert returns 'true' if segment has been added successfully
  //caller must free this packet if it returns 'false'
  bool insert(TCP_Segment* segment, UInt32 seqnum, unsigned len);
  
};

} // namespace ipstack

#endif // __TCP_RECEIVEBUFFER_H__

