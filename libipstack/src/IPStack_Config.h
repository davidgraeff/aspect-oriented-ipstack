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

/**
 * Some variables for the entire ip-stack. The are either provided
 * by kconfig or set to a default value.
 */
namespace ipstack {
	#ifdef kconfig_cfIPSTACK_BLOCKSIZE_BIG
		// *** Default mempool configuration (from kconf)
		enum {
		BLOCKSIZE_BIG = kconfig_cfIPSTACK_BLOCKSIZE_BIG,
		COUNT_BIG = kconfig_cfIPSTACK_COUNT_BIG,
		BLOCKSIZE_SMALL = kconfig_cfIPSTACK_BLOCKSIZE_SMALL,
		COUNT_SMALL = kconfig_cfIPSTACK_COUNT_SMALL
		};
	#else
		// *** Default mempool configuration
		enum {
		BLOCKSIZE_BIG = 1514,
		COUNT_BIG = 4, //a power of 2 is most efficient
		BLOCKSIZE_SMALL = 64,
		COUNT_SMALL = 4 //a power of 2 is most efficient
		};
	#endif

	#ifdef kconfig_cfIPSTACK_MEMORY_GENERIC // from kconfig, default: no
		enum { MEMORY_GENERIC = 1};
	#else
		enum { MEMORY_GENERIC = 0};
	#endif

	/**
	 * This specifies the maximum amount of packets that
	 * can be buffered for each connection. This affects
	 * TCP sending and receiving and UDP receiving.
	 * Code usage: Ringbuffer
	 * A power of 2 is most efficient.
	 */
	
	#ifdef kconfig_cfIPSTACK_PACKET_LIMIT
		enum {PACKET_LIMIT = cfIPSTACK_PACKET_LIMIT};
	#else
		enum {PACKET_LIMIT = COUNT_BIG+COUNT_SMALL};
	#endif
		
	// shared memory
	#ifdef kconfig_cfIPSTACK_MANAGEMENT_BLOCKSIZE_BIG
		// *** Default mempool configuration (from kconf)
		enum {
		IPSTACK_MANAGEMENT_BLOCKSIZE_BIG = kconfig_cfIPSTACK_MANAGEMENT_BLOCKSIZE_BIG,
		IPSTACK_MANAGEMENT_COUNT_BIG = kconfig_cfIPSTACK_MANAGEMENT_COUNT_BIG,
		IPSTACK_MANAGEMENT_BLOCKSIZE_SMALL = kconfig_cfIPSTACK_MANAGEMENT_BLOCKSIZE_SMALL,
		IPSTACK_MANAGEMENT_COUNT_SMALL = kconfig_cfIPSTACK_MANAGEMENT_COUNT_SMALL
		};
	#else
		// *** Default mempool configuration
		enum {
		IPSTACK_MANAGEMENT_BLOCKSIZE_BIG = 1514,
		IPSTACK_MANAGEMENT_COUNT_BIG = 4, //a power of 2 is most efficient
		IPSTACK_MANAGEMENT_BLOCKSIZE_SMALL = 64,
		IPSTACK_MANAGEMENT_COUNT_SMALL = 4 //a power of 2 is most efficient
		};
	#endif

} //namespace ipstack
