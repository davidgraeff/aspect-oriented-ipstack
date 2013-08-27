// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


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
  TCP_RecvElement elements[TCP_RECEIVEBUFFER_MAX_PACKETS];
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
  
  void socket_free(TCP_Segment* segment);
  
  void copyData(void* dst, unsigned len);
  
  //insert returns 'true' if segment has been added successfully
  //caller must free this packet if it returns 'false'
  bool insert(TCP_Segment* segment, UInt32 seqnum, unsigned len);
  
};

} // namespace ipstack

#endif // __TCP_RECEIVEBUFFER_H__

