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
// Copyright (C) 2011 Christoph Borchert, 2012 David Gräff

#pragma once

#include <string.h>
#include "util/types.h"
#include "ipstack/util/Mempool.h"
namespace ipstack
{

/**
 * Inherit this to use the management memory. It is important to call the constructor of this super class.
 * All functionality that inherit this management memory class share the same memory block.
 */
class ManagementMemory
{
	public:
		/**
		 * Initialise the mempool pointer to point to the shared memory.
		 * You really want to call this super class constructor in your class!
		 */
		ManagementMemory();
	protected:
		Mempool* mempool;
};


} //namespace ipstack
