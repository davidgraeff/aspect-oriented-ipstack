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
// Copyright (C) 2012 David Gräff

#pragma once

#include "../router/Interface.h"
#include "util/types.h"

namespace ipstack {

class InternetChecksum {
public:
	static inline UInt16 byteswap16(UInt16 val) {
		return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	}
	static inline UInt16 invert(UInt16 csum, Interface* interface) {
		return ~csum;
	}

	static inline UInt16 accumulateCarryBits(UInt32 csum) {
		while (csum >> 16) {
			csum = (csum & 0xFFFF) + (csum >> 16); // accumulate carry bits
		}
		return csum;
	}
	static UInt32 computePayload(UInt8* payloaddata, UInt16 payloadlen) {
		//Software Checksumming
		UInt32 csum = 0;

		if (payloadlen & 0x1) {
			//odd datasize: add padding byte (zero)
			UInt16 odd = payloaddata[payloadlen - 1];
			csum += (odd & 0xFF) << 8;
		}

		payloadlen /= 2;

		UInt16* payload = (UInt16*) payloaddata;
		for (unsigned i = 0; i < payloadlen; ++i) {
			csum += byteswap16(*payload++);
		}
		return csum;
	}

	static UInt16 computePayloadChecksum(UInt8* payloaddata, UInt16 payloadlen) {
		UInt32 csum = computePayload(payloaddata, payloadlen);
		return (~accumulateCarryBits(csum)); // one's complement
	}
};

} //namespace ipstack

