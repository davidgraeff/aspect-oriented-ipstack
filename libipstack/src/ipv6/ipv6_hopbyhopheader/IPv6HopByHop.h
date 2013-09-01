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

#include "ipv6/IPv6AddressUtilities.h"
#include "ipv6/IPv6_Packet.h"
#include "ipv6/IPv6onSockets.h"
#include "util/ipstack_inttypes.h"
#include "../ExtensionHeaderOptions.h"
namespace ipstack
{
class IPv6HopByHop
{
	private:
		/**
		 * HopByHop Option aspects weave here
		 */
		static unsigned getMinSize(ipstack::IPV6& ipv6) {
			return 0;
		}
		/**
		 * HopByHop Option aspects weave here. Return used size
		 */
		static unsigned writeOptions(IPV6& ipv6, uint8_t* data) {
			return 0;
		}

	public:
		enum {IPV6_NEXTHEADER_HOPBYHOP = 0};
		static unsigned writeExtensionHeader(IPV6& ipv6, char* data) {
			// No hop by hop options -> abort
			if (getMinSize(ipv6)==0)
				return 0;
			
			IPv6HeaderExtension* e = (IPv6HeaderExtension*)data;
			unsigned all_size = getSize(ipv6);
			unsigned used_size = writeOptions(ipv6, e->options);
			e->len_in_8octets = all_size / 8 - 1; // size in octects but without the first octet
			
			used_size = all_size - used_size; // := remaining size
			if (used_size) {
				ExtensionHeaderOptions::writePaddingOption(((uint8_t*)e->options)+used_size, (uint8_t)used_size- sizeof(IPv6HeaderExtension));
			}
			return all_size;
		}
		static unsigned getSize(ipstack::IPV6& ipv6) {
			unsigned minsize = getMinSize(ipv6);
			// No options configured
			if (!minsize)
				return 0;
			// We have a min size. But we need an aligned size. Each extension header has to be 64bit (8 Byte) aligned.
			uint8_t rest = minsize % 8;
			if (!rest)
				return minsize;
			else
				return minsize + (8-rest);
		}
};
}
