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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "../cfAttribs.h"

namespace ipstack {
// Call releaseOldEntries() method of the ndp cache every NDP_CACHE_REFRESH_IN_MINUTES minutes!
// NDPCACHE_EXPIRE_RATE have to be an even number!
enum {NDP_CACHE_REFRESH_IN_MINUTES = 3, NDPCACHE_EXPIRE_RATE_PER_MINUTE = 2 * NDP_CACHE_REFRESH_IN_MINUTES,
	  NDPCACHE_TRESHOLD_REQUIRE_SOLICITATION = NDPCACHE_EXPIRE_RATE_PER_MINUTE * 3
	 };
}
