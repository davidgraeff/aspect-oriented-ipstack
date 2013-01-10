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

namespace ipstack {

// Return 18 bytes: mac address as string
void ethernet_addr_tostring(char *dst, UInt8* eth_addr);
/**
 * Parse a textual ethernet address representation like "20:01:0d:b8:00:00" into the native representation
 * @param addrstr Textual address representation
 * @param ipaddr Resulting native ethernet represenation
 * @return Return true if successful otherwise false.
 */
bool parse_ethernet_addr(const char *addrstr, UInt8* eth_addr);

} //namespace ipstack
