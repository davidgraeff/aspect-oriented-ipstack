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
// Copyright (C) 2013 David Gräff

#pragma once
#include "Mempool_Config.h"
#include "HeapMemory.h"
#include "Mempool.h"

/**
 * Some variables for the entire ip-stack. They are either provided
 * by kconfig or set to a default value.
 */
namespace ipstack {
	// Management Sockets Memory Type
	#ifdef kconfig_ipstack_memory_stack_bigsmall // from kconfig
		// two types of slots (big/small) in a mempool
		typedef Mempool<IPSTACK_MANAGEMENT_BLOCKSIZE_BIG,IPSTACK_MANAGEMENT_COUNT_BIG,
		IPSTACK_MANAGEMENT_BLOCKSIZE_SMALL,IPSTACK_MANAGEMENT_COUNT_SMALL> ManagementSocketMemory;
		#define ipstack_memory_defined
	#endif
	#ifdef kconfig_ipstack_memory_stack_bigonly // from kconfig
		// one type of slots in a mempool
		typedef Mempool<IPSTACK_MANAGEMENT_BLOCKSIZE_BIG,IPSTACK_MANAGEMENT_COUNT_BIG> ManagementSocketMemory;
		#define ipstack_memory_defined
	#endif
	#ifdef kconfig_ipstack_memory_heap // from kconfig
		// heap memory
		typedef HeapMemory ManagementSocketMemory;
		#define ipstack_memory_defined
	#endif
	#ifndef ipstack_memory_defined
		// default for non-kconfig setups
		typedef Mempool<IPSTACK_MANAGEMENT_BLOCKSIZE_BIG,IPSTACK_MANAGEMENT_COUNT_BIG,
		IPSTACK_MANAGEMENT_BLOCKSIZE_SMALL,IPSTACK_MANAGEMENT_COUNT_SMALL> ManagementSocketMemory;
	#endif

} //namespace ipstack
