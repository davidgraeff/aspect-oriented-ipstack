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


#ifndef __TCP_HISTORY_H__
#define __TCP_HISTORY_H__

#include <inttypes.h>
#include "../TCP.h"
#include "../TCP_Config.h"
#include "TCP_Record.h"
#include "SendBuffer.h"

namespace ipstack {


class TCP_History{
  private:
  TCP_Record records[TCP_HISTORY_MAX_PACKETS];
  TCP_Record* head; //doubly linked (for quick remove) if TCP_History_SlidingWindow.ah

  public:
  TCP_History() : head(0) {}
  
  void add(SendBuffer* b, uint32_t msec);
  
  TCP_Record* get() { return head; }
  bool isEmpty() { return (head == 0); }
  bool isFull() { return !isEmpty(); }
  
  void remove(TCP_Record* record);
  
  uint32_t getNextTimeout(); // relative value in msecs

};

} // namespace ipstack

#endif // __TCP_HISTORY_H__

