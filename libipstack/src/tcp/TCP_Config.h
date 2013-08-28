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

#pragma once
#include "cfAttribs.h"

namespace ipstack {
	//This is the maximum number of retransmissions for a TCP segment.
	//The connection is closed if any segment exceeds this number.
	//Values range from 3 to 255.
	#ifdef kconfig_cfTCP_MAX_RETRANSMISSIONS //kconfig value
		enum {TCP_MAX_RETRANSMISSIONS=kconfig_cfTCP_MAX_RETRANSMISSIONS};
	#else
		enum {TCP_MAX_RETRANSMISSIONS=10};
	#endif
	
	//This is the maximum amount of packets that can be stored in the
	//tcp receivebuffer foreach connection
	//set this value to 1 if you don't use a sliding receive window
	//and if you don't use kconfig
	#ifdef kconfig_ipstack_tcp_slidingwnd_recv //kconfig value
		enum {TCP_RECEIVEBUFFER_MAX_PACKETS=PACKET_LIMIT};
	#else
		enum {TCP_RECEIVEBUFFER_MAX_PACKETS=1};
	#endif
	
	//This is the maximum amount of packets that can be send simultaneously
	//foreach tcp connection
	//set this value to 1 if you don't use a sliding send window
	//and if you don't use kconfig
	#ifdef kconfig_ipstack_tcp_slidingwnd_recv //kconfig value
		enum {TCP_HISTORY_MAX_PACKETS=PACKET_LIMIT};
	#else
		enum {TCP_HISTORY_MAX_PACKETS=1};
	#endif
}
