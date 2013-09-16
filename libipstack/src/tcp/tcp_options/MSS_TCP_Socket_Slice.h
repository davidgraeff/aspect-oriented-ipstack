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

slice class MSS_TCP_Socket_Slice{
public:
	void recalculate_mss() {
		// estimateSendBufferMinSize MUST NOT be influenced by the tcp header size aspect in this context
		const uint_fast16_t headersize = estimateSendBufferMinSize();
		const uint_fast16_t memsize = get_Mempool()->SIZE_BIG;
		const uint_fast16_t mtusize = getMTU();
		if (memsize<mtusize)
			setMSS(memsize-headersize);
		else
			setMSS(mtusize-headersize);
	}
};
