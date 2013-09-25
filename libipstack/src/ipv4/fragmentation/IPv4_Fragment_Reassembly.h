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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include "util/ipstack_inttypes.h"

/**
 * TODO:
 * * link IPv4_Fragment_Reassembly together. We need one IPv4_Fragment_Reassembly
 *   object for every unique Identification.
 * * Determine right IPv4_Fragment_Reassembly object for each received fragmented packet.
 * * Timeout handling
 * * Freeing ReceiveBuffer for timeouts or handover reassambled packets is only possible if
 *   socket is known! (SocketMemory class!) -> fortunately we know the SocketMemory object because
 *   we intercept addToReceiveQueue.
 * * Allocation of a smaller buffer is necessary for each IPv4_Fragment_Reassembly for the
 *   bitmap part
 * * If no fragmentation support -> we may want to send an icmp message (dest_not_reachable:size)
 */
namespace ipstack {
	class IPv4_Fragment_Reassembly {
	public:
		IPv4_Fragment_Reassembly();
		void clear();
		void add_fragmented_data(ReceiveBuffer* new_data_buffer);
	private:
		enum { TIMEOUT = 60 }; //seconds, within next fragment must arrive;
		uint64_t expired_time;
		ReceiveBuffer* receivebuffer;
		uint8_t* bitmap; // [BUFFERSIZE / (8 * 8)];
		uint8_t header_len; // set when first fragment arrives
		uint16_t total_len; // 0 until last fragment received
		bool full_bitmap();
		void update_bitmap(unsigned start, unsigned end);
	};
} //namespace ipstack
