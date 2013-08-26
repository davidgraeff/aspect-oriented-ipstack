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
// Copyright (C) 2012 David Gräff

#pragma once

#include "SendBuffer.h"
namespace ipstack {

class DelaySend
{
	public:
		/**
		* In contrast to all other managment functionality an ICMPv6 incoming packet may generate
		* multiple output packets. To have a deterministic order of processing we delay all
		* send() calls until the packet has been processed and send all delayed packets in a burst.
		* The mHoldBack variable will be set before processing a packet and will be reseted after that.
		*/
		static UInt8 mHoldBack;

		static void sendDelayed(SendBuffer* buffer);
};

}