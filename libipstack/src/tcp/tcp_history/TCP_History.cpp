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


#include "TCP_History.h"

namespace ipstack {

	void TCP_History::add(SendBuffer* b, UInt32 msec){ //msec -> timeout for rexmit
  //be sure to check isFull _BEFORE_ using this function!
  
  head = &records[0];
  //fill in data
  head->setSendBuffer(b);
  head->setTimeout(msec);
}

void TCP_History::remove(TCP_Record* record){
  head = 0;
}

UInt32 TCP_History::getNextTimeout(){ // relative value in msecs
  if(head == 0){
    return 0;
  }
  else{
    return head->getRemainingTime();
  }
}

} // namespace ipstack

