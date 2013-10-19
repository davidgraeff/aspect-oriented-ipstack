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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff

#pragma once
#include <inttypes.h>
#include <sys/time.h>
#include "os_integration/aspectless_api/os_integration.h"
namespace ipstack_app {

	uint64_t Clock::now() {
		struct timeval tv;
		struct timezone tz;

		gettimeofday(&tv, &tz);

		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	uint64_t Clock::ms_to_ticks(uint32_t ms) { return ms; }
	uint32_t Clock::ticks_to_ms(uint64_t ticks) { return ticks; }

} // end namespace
