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
#include "ip/ip_socket/RawIP_Socket.h"

namespace ipstack
{
	/**
	 * We define an own class for icmpv6 traffic, to have more context for aspect matching.
	 * For example the checksum calculation is done via an aspect. We do need to call this
	 * socket _DerivedSocket otherwise aspects for sending/receiving would try extend this
	 * class.
	 */
	class ICMPv6_DerivedSocket: public RawIP_Socket {
	private:
		// no copies
		ICMPv6_DerivedSocket(const ICMPv6_DerivedSocket&) {}
	public:
		ICMPv6_DerivedSocket(const SocketMemory& memory) : RawIP_Socket(memory) {}
	};
}