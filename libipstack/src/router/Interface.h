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
// Copyright (C) 2011-2012 Christoph Borchert, David Gräff

#pragma once
#include "util/ipstack_inttypes.h"

namespace ipstack {

class Interface {
  private:
  Interface* next_interface; //linked list

  public:
  enum { ETHERNET = 1,
         LOOPBACK = 2,
         SLIP = 3 };
  
  Interface* getNext() { return next_interface; }
  
  //these functions have to be delegated to network device abstraction
  // -> performed by aspect Interface.ah
  const char* getName() { return 0; }
  unsigned char getType() { return 0; }
  const unsigned char* getAddress() { return 0; }
  unsigned getMTU() { return 0; }
  uint8_t getAddressSize() { return 0; }
  
  bool hasTransmitterHardwareChecksumming() { return false; }
  bool hasReceiverHardwareChecksumming() { return false; }
  
  void send(const void* frame, unsigned framesize) {}
  // For checksum offloading and networc card send offloading
  void send(const void* data, unsigned datasize, bool tcp, unsigned ip_hdl) {}
  bool hasBeenSent(const void* frame){ return true; }

};

} // namespace ipstack
