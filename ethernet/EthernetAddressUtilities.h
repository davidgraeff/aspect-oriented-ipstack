// This file is part of CiAO/IPv6-Extension.
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
// along with CiAO/IPv6-Extension.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2012 David Gräff

#pragma once
/*
 * Purpose: Ethernet address conversions from string to binary represenation. These utility method
 * should be optimized out if not used by the compiler and do not need a seperate aspects.
 */

#include "util/types.h"
#include "Eth_Frame.h"

namespace ipstack {

	UInt8 numberFromAsciiE(char c) {
		switch (c) {
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;
			case 'a': return 10;
			case 'A': return 10;
			case 'b': return 11;
			case 'B': return 11;
			case 'c': return 12;
			case 'C': return 12;
			case 'd': return 13;
			case 'D': return 13;
			case 'e': return 14;
			case 'E': return 14;
			case 'f': return 15;
			case 'F': return 15;
			default:
				return 0;
		};
	}
	
/**
 * Parse a textual ethernet address representation like "20:01:0d:b8:00:00" into the native representation
 * @param addrstr Textual address representation
 * @param ipaddr Resulting native ethernet represenation
 * @return Return true if successful otherwise false. ipaddr is not altered if the parsing was not successful
 */
bool parse_ethernet_addr(const char *addrstr, UInt8& eth_addr[])
{
	UInt8 blockIndex = 0;
	char* currentCharPtr = (char*)addrstr;
	while (blockIndex < 6) {
		UInt8 blockdata[2] = {0};
		UInt8 position = 0;
		//printf("block ");
		while(*currentCharPtr != ':') {
			//printf ("%c", *currentCharPtr);
			blockdata[position] = numberFromAsciiE(*currentCharPtr);
			++currentCharPtr;
			if (++position > 1)
				break;
		}
		eth_addr[blockIndex] = (blockdata[1]) | (blockdata[0] << 4);
		if (*currentCharPtr == 0)
			break;
		++currentCharPtr;
		++blockIndex;
	}
	if (blockIndex!=6)
		return 0;
	return 1;
}

} //namespace ipstack
