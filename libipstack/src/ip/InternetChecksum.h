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

#include "router/Interface.h"
#include "util/ipstack_inttypes.h"

namespace ipstack {

class InternetChecksum {
public:
	static inline uint16_t byteswap16(uint16_t val) {
		return ((val & 0xFF) << 8) | ((val & 0xFF00) >> 8);
	}
	static inline uint16_t invert(uint16_t csum, Interface* interface) {
		return ~csum;
	}

	static inline uint16_t accumulateCarryBits(uint32_t csum) {
		while (csum >> 16) {
			csum = (csum & 0xFFFF) + (csum >> 16); // accumulate carry bits
		}
		return csum;
	}
	static uint32_t computePayload(uint8_t* payloaddata, uint16_t payloadlen) {
		//Software Checksumming
		uint32_t csum = 0;

		if (payloadlen & 0x1) {
			//odd datasize: add padding byte (zero)
			uint16_t odd = payloaddata[payloadlen - 1];
			csum += (odd & 0xFF) << 8;
		}

		payloadlen /= 2;

		uint16_t* payload = (uint16_t*) payloaddata;
		for (unsigned i = 0; i < payloadlen; ++i) {
			csum += byteswap16(*payload++);
		}
		return csum;
	}

	static uint16_t computePayloadChecksum(uint8_t* payloaddata, uint16_t payloadlen) {
		uint32_t csum = computePayload(payloaddata, payloadlen);
		return (~accumulateCarryBits(csum)); // one's complement
	}
	
	static uint32_t computePseudoHeader(char* ip_packet, uint16_t payloadlen, uint8_t upper_layer_nextheader) {
		// weave in checksum calculation for ip header
	}
};

} //namespace ipstack

