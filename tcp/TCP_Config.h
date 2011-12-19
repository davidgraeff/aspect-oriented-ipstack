// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


#ifndef __TCP_CONFIG_H__
#define __TCP_CONFIG_H__

#include "../IPStack_Config.h"

//This is the maximum number of retransmissions for a tcp segment.
//The connection is closed if any segment exceeds this number.
//Values range from 3 to 255.
#ifndef __TCP_MAX_RETRANSMISSIONS__
  #define __TCP_MAX_RETRANSMISSIONS__ 10
#endif


//This is the maximum amount of packets that can be stored in the
//tcp receivebuffer foreach connection
#ifndef __TCP_RECEIVEBUFFER_MAX_PACKETS__
  #define __TCP_RECEIVEBUFFER_MAX_PACKETS__ __IPSTACK_MAX_PACKETS__
  //#define __TCP_RECEIVEBUFFER_MAX_PACKETS__ 10 //user customization possible
  //set this value to 1 if you don't use a sliding receive window
  //and if you don't use kconfig
#endif

//This is the maximum amount of packets that can be send simultaneously
//foreach tcp connection
#ifndef __TCP_HISTORY_MAX_PACKETS__
  #define __TCP_HISTORY_MAX_PACKETS__ __IPSTACK_MAX_PACKETS__
  //#define __TCP_HISTORY_MAX_PACKETS__ 5 //user customization possible
  //set this value to 1 if you don't use a sliding send window
  //and if you don't use kconfig
#endif

#endif // __TCP_CONFIG_H__

