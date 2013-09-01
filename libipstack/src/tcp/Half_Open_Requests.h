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

namespace ipstack
{

/**
 * Allows TCP Management Services like half-open tcp request responding.
 */
class SendBuffer;
class TCP_Segment;
class Half_Open_Requests
{
	public:
	/**
	* Prepare a sendbuffer for answering half-open tcp requests
	*/
	static void prepareResponse(SendBuffer* sendbuffer, TCP_Segment* incoming_segment, uint_fast16_t payload_len);
};

} //namespace ipstack
