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
namespace ipstack
{

/**
  * The structure of an IPv6 extension
  */
struct IPv6HeaderExtension {
	uint8_t nextheader;		// Identifies the type of the header that will follow this extension header
	uint8_t len_in_8octets; 	// example: 1 means a size of 64 bits
	uint8_t options[];
} __attribute__((packed));

/**
 * Parses extension header options and provide padding options
 */
class ExtensionHeaderOptions
{
public:
	/**
	 * Return false if a parsing error occured or if one of the options
	 * told us, that if we do not understand it, we should drop the packet.
	 */
	static bool parseOptions(uint8_t* data, unsigned remaining_length, Interface* interface) {
		while (remaining_length) {
			uint8_t option_len = data[1];
			if (!parseOption(data[0], option_len, &data[2], interface))
				return false;
			
			if (option_len>remaining_length)
				return false;
			remaining_length -= option_len;
			data += option_len;
		}
		return true;
	}
	
	/**
	 * This can be intercepted by aspects to react on received IPv6 options.
	 */
	static bool parseOption(uint8_t type, uint8_t data_len, uint8_t* data, Interface* interface) {
		// We didn't recognice this option and the option want us to drop the packet
		if ((type & 0xC0) != 00)
			return false;
		return true;
	}
	
	/**
	  * Extensions always have to be multiples of 8 octets (64 bit). We have to pad 
	  * not used space after a too-short option. There exists either the one byte
	  * padding (value=0) or the multple bytes padding (first byte=1, second byte=len-2)
	  */
	static void writePaddingOption(uint8_t* data, uint8_t remainingBytes) {
		if (remainingBytes==1) {
			data[0] = 0;
		} else {
			data[0] = 1;
			data[1] = remainingBytes-2;
		}
	}
};

} //namespace ipstack
