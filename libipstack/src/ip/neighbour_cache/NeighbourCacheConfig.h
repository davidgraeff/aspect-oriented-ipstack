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

#include "IPStack_Config.h"

namespace ipstack {
	#ifdef kconfig_cfNEIGHBOURCACHE_ENTRIES //kconfig value
		enum {NEIGHBOURCACHE_ENTRIES=kconfig_cfNEIGHBOURCACHE_ENTRIES};
	#else
		enum {NEIGHBOURCACHE_ENTRIES = 10U};
	#endif
		
	#ifdef kconfig_cfNEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS //kconfig value
		enum {NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS=kconfig_cfNEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS};
	#else
		enum {NEIGHBOURCACHE_WAIT_FOR_RESPONSE_MS = 1000U};
	#endif
	
	// BSD: if_ether.c: /* once resolved, good for 20 more minutes */
	#ifdef kconfig_cfNEIGHBOURCACHE_GLOBAL_TIMEOUT_SEC //kconfig value
		enum {NEIGHBOURCACHE_GLOBAL_TIMEOUT_SEC=kconfig_cfNEIGHBOURCACHE_GLOBAL_TIMEOUT_SEC};
	#else
		enum {NEIGHBOURCACHE_GLOBAL_TIMEOUT_SEC = 20*60}; // in seconds
	#endif
	
	#ifdef kconfig_cfNDP_CACHE_REFRESH_SEC //kconfig value
		enum {NDP_CACHE_REFRESH_SEC=kconfig_cfNDP_CACHE_REFRESH_SEC};
	#else
		enum {NDP_CACHE_REFRESH_SEC = 3*60}; // in seconds
	#endif
	enum {
		NDPCACHE_EXPIRE_RATE_PER_MINUTE = 2 * NDP_CACHE_REFRESH_SEC,
		NDPCACHE_TRESHOLD_REQUIRE_SOLICITATION = NDPCACHE_EXPIRE_RATE_PER_MINUTE * 3
	};
}
