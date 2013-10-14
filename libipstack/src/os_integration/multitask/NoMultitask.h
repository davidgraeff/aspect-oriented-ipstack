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
#include "util/ipstack_inttypes.h"

/**
 * You have to implement the following classes
 */
namespace ipstack {
	class NoMultitask {
	public:
		/**
		 * Some libipstack functionalities like arp, ndp and tcp are using
		 * busy waiting. If you have configured libipstack to be single tasked,
		 * the wait_for_input method is called in the busy loops to provide you
		 * a mean to receive data from the network and provide it to the ipstack.
		 * 
		 */
		static void wait_for_input() {}
	};
} // ipstack
