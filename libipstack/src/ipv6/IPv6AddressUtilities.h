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
 * Purpose: IPv6 address conversions from and to different formats and IPv6-Address scope. These utility methods
 * should be optimized out if not used by the compiler and do not need seperate aspects.
 */

#include "util/types.h"
#include <string.h>

#define IP_STR_VALUE(arg)      #arg
#define IP_MAKE_STRING(name) IP_STR_VALUE(name)
//#define IP_MAKE_STRING(name) #name

namespace ipstack {

// IPv6 address representations
union ipv6addr {
	// byte[0] is the most "left", byte[15] the most "right"
	// Example: "2001:0db8:0000:08d3:0000:8a2e:0070:7344" -> byte[0]==0x20, byte[15]==0x44
	UInt8 ipaddrB8[16]; // representation: byte array
// 	UInt16 ipaddrB16[8]; // representation: byte array
};

namespace IPV6AddressState {
enum IPv6_ADDRESS_STATE {
	IPV6_ADDRESS_PRELIMINARY,
	IPV6_ADDRESS_OK
};
}
namespace IPV6AddressScope {
enum IPv6_ADDRESS_SCOPE {
	IPV6_SCOPE_NONDEFINED, // :0 the not defined IP
	IPV6_SCOPE_LOOPBACK,   // :1 loopback IP
	IPV6_SCOPE_LINKLOCAL,  // fe80::/64
	IPV6_SCOPE_UNIQUE_LOCAL_UNICAST, // fc00::/7 privat addresses (fc... global unqiue; fd... site unqiue)
	IPV6_SCOPE_MULTICAST, // ff00::/8 multicast
	IPV6_SCOPE_GLOBAL_UNICAST, // 0:0:0:0:0:ffff::/96 IPv4 mapped; 2000::/3 global unqiue addresses; there are more prefixes for this scope but not fully defined so far
	IPV6_SCOPE_NOT_RECOGNICED
};
UInt8 getIPv6AddressScopePrefixLength(IPv6_ADDRESS_SCOPE s);
IPv6_ADDRESS_SCOPE getIPv6AddressScope(const ipstack::ipv6addr& addr);
}

/**
  * Build the corresponding  multicast address for a link local address
  * src_ip_addr: The link local address
  * solicitedNode: The destination multicast address
  */
void solicitedNode_multicast_addr(const ipstack::ipv6addr& src_ip_addr, ipv6addr& solicitedNode);

/**
 * Copy an IPv6 address
 */
void copy_ipv6_addr(const ipstack::ipv6addr& src, ipstack::ipv6addr& dest);

/**
 * Get unspecified address (::/128)
 */
void get_unspecified_ipv6_address(ipstack::ipv6addr& dest);
bool is_not_unspecified_ipv6_address(const ipstack::ipv6addr& dest);

/**
 * If addr matches the addrprefix with prefixlen return true
 */
bool compare_ipv6_addr(const ipstack::ipv6addr& addr, const ipstack::ipv6addr& addrprefix, UInt8 prefixlen);
/**
 * A fast way to compare, if all bytes have to be compared.
 */
bool compare_ipv6_addr(const ipstack::ipv6addr& addr, const ipstack::ipv6addr& addr2);

/**
 * Parse a textual ipv6 address representation like "2001:0db8:0000:08d3:0000:8a2e:0070:7344" into the native representation.
 * A short variant is also possible, where leading 0's are left out, like "2001:db8::08d3::8a2e:70:7344".
 * @param addrstr Textual address representation
 * @param ipaddr Resulting native ipv6 represenation
 * @return Return true if successful otherwise false. ipaddr is not altered if the parsing was not successful
 */
bool parse_ipv6_addr(const char* addrstr, ipstack::ipv6addr& ipaddr);

/**
 * Parse a native representation into the textual ipv6 address representation like "2001:0db8:0000:08d3:0000:8a2e:0070:7344".
 * This method also parses strings where leading 0 are ommited like "2001:db8::8d3::8a2e:70:7344". But there always have to
 * be 8 blocks separated by ":".
 * @param ipaddr Native ipv6 represenation
 * @param addrstr Memory for the textual address representation. 39 characters are generated.
 */
void ipv6_addr_toString(const ipstack::ipv6addr& ipaddr, char* addrstr);

} //namespace ipstack
