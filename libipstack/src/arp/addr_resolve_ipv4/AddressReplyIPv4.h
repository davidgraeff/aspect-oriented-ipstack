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
#include <string.h> //for memcpy
#include "util/ipstack_inttypes.h"
#include "ethernet/Eth_Frame.h"
#include "router/Interface.h"
#include "os_integration/Clock.h"

#include "ARP_IPv4_Packet.h"
#include "arp/ARP_Packet.h"


namespace ipstack {

class AddressReplyIPv4 {
	public:
	// handle incoming arp request: return an arp reply packet to the requesting host 
	static inline void reply(const uint8_t* src_hwaddr, const uint32_t* src_ipv4_addr, SmartReceiveBufferPtr& b);
};

} // namespace ipstack
