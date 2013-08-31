// This file is part of Aspect-Oriented-IPv6-Extension.
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
// along with Aspect-Oriented-IPv6-Extension.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2012 David Gräff

#pragma once
/*
 * Purpose: Ethernet address conversions from string to binary represenation. These utility method
 * should be optimized out if not used by the compiler and do not need a seperate aspects.
 */

#include "util/ipstack_inttypes.h"

namespace ipstack {

/**
 * Parse a native representation into the textual mac address representation like "20:b8:00:d3:00:2e".
 * @param eth_addr Native mac represenation
 * @param addrstr Memory for the textual address representation. 18 characters are generated.
 */
void ethernet_addr_tostring(uint8_t* eth_addr, char *addrstr);
/**
 * Parse a textual ethernet address representation like "20:01:0d:b8:00:00" into the native representation
 * @param addrstr Textual address representation
 * @param ipaddr Resulting native ethernet represenation
 * @return Return true if successful otherwise false.
 */
bool parse_ethernet_addr(const char *addrstr, uint8_t* eth_addr);

/**
  * Return true if a valid mac addr is set. The first three bytes are
  * valid if they are listed in http://standards.ieee.org/develop/regauth/oui/oui.txt.
  * For simplification we do not test the first three bytes (even 00-00-00 is assigned and therefore valid).
  * The second three bytes of the mac address are usually nic identifier of the network card and
  * should be != 0. This method test the second three bytes.
  */
bool is_eth_addr_set(uint8_t* eth_addr);
} //namespace ipstack
