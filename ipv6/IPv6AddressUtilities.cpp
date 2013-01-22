#include "IPv6AddressUtilities.h"

namespace ipstack {
namespace IPV6AddressScope {
UInt8 getIPv6AddressScopePrefixLength(ipstack::IPV6AddressScope::IPv6_ADDRESS_SCOPE s)
{
	switch (s) {
		case IPV6_SCOPE_LINKLOCAL:
			return 64;
		case IPV6_SCOPE_GLOBAL_UNICAST:
			return 3;
		case IPV6_SCOPE_MULTICAST:
			return 8;
		case IPV6_SCOPE_UNIQUE_LOCAL_UNICAST:
			return 7;
		default:
			return 0;
	};
}
IPv6_ADDRESS_SCOPE getIPv6AddressScope(const ipv6addr& addr)
{
	// 2000::/3 global unqiue addresses;
	if ((addr.ipaddrB8[0] & 0x30) == 1)
		return IPV6_SCOPE_GLOBAL_UNICAST;

	// ff00::/8 multicast
	if (addr.ipaddrB8[0] == 0xff)
		return IPV6_SCOPE_MULTICAST;

	// fe80::/64 link local
	if ((addr.ipaddrB8[0] == 0xfe) && (addr.ipaddrB8[1] == 0x80))
		return IPV6_SCOPE_LINKLOCAL;

	// fc00::/7 privat addresses (fc... global unqiue; fd... site unqiue)
	if ((addr.ipaddrB8[0] == 0xfc) || (addr.ipaddrB8[0] == 0xfd))
		return IPV6_SCOPE_UNIQUE_LOCAL_UNICAST;

	// first 10 bytes == 0
	if ((addr.ipaddrB8[0] == 0) && (addr.ipaddrB8[1] == 0) && (addr.ipaddrB8[2] == 0) && (addr.ipaddrB8[3] == 0)
			&& (addr.ipaddrB8[4] == 0) && (addr.ipaddrB8[5] == 0) && (addr.ipaddrB8[6] == 0) && (addr.ipaddrB8[7] == 0)
			&& (addr.ipaddrB8[8] == 0) && (addr.ipaddrB8[9] == 0)) {
		// 0:0:0:0:0:ffff::/96 IPv4 mapped;
		if ((addr.ipaddrB8[10] == 0xff) && (addr.ipaddrB8[11] == 0xff))
			return IPV6_SCOPE_GLOBAL_UNICAST;

		// first 15 bytes == 0
		if ((addr.ipaddrB8[10] == 0) && (addr.ipaddrB8[12] == 0) && (addr.ipaddrB8[13] == 0) && (addr.ipaddrB8[14] == 0)) {
			// :0 non defined address
			if (addr.ipaddrB8[15] == 0)
				return IPV6_SCOPE_NONDEFINED;
			// :1 loopback
			else if (addr.ipaddrB8[15] == 1)
				return IPV6_SCOPE_LOOPBACK;
		}
	}

	return IPV6_SCOPE_NOT_RECOGNICED;
}
}
void copy_ipv6_addr(const ipv6addr& src, ipv6addr& dest)
{
// 	dest.ipaddrB8[0] = src.ipaddrB8[0];
// 	dest.ipaddrB8[1] = src.ipaddrB8[1];
// 	dest.ipaddrB8[2] = src.ipaddrB8[2];
// 	dest.ipaddrB8[3] = src.ipaddrB8[3];
// 	dest.ipaddrB8[4] = src.ipaddrB8[4];
// 	dest.ipaddrB8[5] = src.ipaddrB8[5];
// 	dest.ipaddrB8[6] = src.ipaddrB8[6];
// 	dest.ipaddrB8[7] = src.ipaddrB8[7];
// 	dest.ipaddrB8[8] = src.ipaddrB8[8];
// 	dest.ipaddrB8[9] = src.ipaddrB8[9];
// 	dest.ipaddrB8[10] = src.ipaddrB8[10];
// 	dest.ipaddrB8[11] = src.ipaddrB8[11];
// 	dest.ipaddrB8[12] = src.ipaddrB8[12];
// 	dest.ipaddrB8[13] = src.ipaddrB8[13];
// 	dest.ipaddrB8[14] = src.ipaddrB8[14];
// 	dest.ipaddrB8[15] = src.ipaddrB8[15];
	memcpy(dest.ipaddrB8, src.ipaddrB8, 16);
}
void get_unspecified_ipv6_address(ipv6addr& dest)
{
	dest.ipaddrB8[0] = 0;
	dest.ipaddrB8[1] = 0;
	dest.ipaddrB8[2] = 0;
	dest.ipaddrB8[3] = 0;
	dest.ipaddrB8[4] = 0;
	dest.ipaddrB8[5] = 0;
	dest.ipaddrB8[6] = 0;
	dest.ipaddrB8[7] = 0;
	dest.ipaddrB8[8] = 0;
	dest.ipaddrB8[9] = 0;
	dest.ipaddrB8[10] = 0;
	dest.ipaddrB8[11] = 0;
	dest.ipaddrB8[12] = 0;
	dest.ipaddrB8[13] = 0;
	dest.ipaddrB8[14] = 0;
	dest.ipaddrB8[15] = 0;
}
bool is_not_unspecified_ipv6_address(const ipv6addr& dest)
{
	char u = 0;
	for (UInt8 i = 0; i < 16; ++i)
		u |= dest.ipaddrB8[i];
	return (bool)u;
}

void solicitedNode_multicast_addr(const ipstack::ipv6addr& src_ip_addr, ipv6addr& solicitedNode) {
	solicitedNode.ipaddrB8[0] = 0xff;
	solicitedNode.ipaddrB8[1] = 0x02;
	solicitedNode.ipaddrB8[2] = 0;
	solicitedNode.ipaddrB8[3] = 0;
	solicitedNode.ipaddrB8[4] = 0;
	solicitedNode.ipaddrB8[5] = 0;
	solicitedNode.ipaddrB8[6] = 0;
	solicitedNode.ipaddrB8[7] = 0;
	solicitedNode.ipaddrB8[8] = 0;
	solicitedNode.ipaddrB8[9] = 0;
	solicitedNode.ipaddrB8[10] = 0;
	solicitedNode.ipaddrB8[11] = 0x01;
	solicitedNode.ipaddrB8[12] = 0xff;
	solicitedNode.ipaddrB8[13] = src_ip_addr.ipaddrB8[13];
	solicitedNode.ipaddrB8[14] = src_ip_addr.ipaddrB8[14];
	solicitedNode.ipaddrB8[15] = src_ip_addr.ipaddrB8[15];
}

UInt8 numberFromAscii(char c)
{
	switch (c) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
			return 10;
		case 'A':
			return 10;
		case 'b':
			return 11;
		case 'B':
			return 11;
		case 'c':
			return 12;
		case 'C':
			return 12;
		case 'd':
			return 13;
		case 'D':
			return 13;
		case 'e':
			return 14;
		case 'E':
			return 14;
		case 'f':
			return 15;
		case 'F':
			return 15;
		default:
			return 0;
	};
}

UInt8 numberToAscii(UInt8 digit)
{
	switch (digit) {
		case 0:
			return '0';
		case 1:
			return '1';
		case 2:
			return '2';
		case 3:
			return '3';
		case 4:
			return '4';
		case 5:
			return '5';
		case 6:
			return '6';
		case 7:
			return '7';
		case 8:
			return '8';
		case 9:
			return '9';
		case 10:
			return 'a';
		case 11:
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		case 15:
			return 'f';
		default:
			return ' ';
	};
}

bool compare_ipv6_addr(const ipv6addr& addr, const ipv6addr& addrprefix, UInt8 prefixlen)
{
	for (UInt8 block = 0; block < 16; ++block) {
		// entire char has to be the same
		if (prefixlen >= 8) {
			if (addr.ipaddrB8[block] != addrprefix.ipaddrB8[block]) {
				return false;
			}
			prefixlen -= 8;
			if (prefixlen == 0)
				return true;
		} else { // only part of the char has to be the same
			/**
			 * Calculate the mask:
			 * 1.) 2^prefixlen -1: If prefixlen is 5 for example: we have 00011111: Five 1's.
			 * 2.) Because we want the 1's starting on the LSB, because we compare from "left to right"
			 *     we use 7-prefixlen (exmpl: 00000111) and invert the result (exmpl: 11111000)
			 */
			UInt8 mask = ~((2 ^(7 - prefixlen)) - 1);
			if ((addr.ipaddrB8[block] & mask) != (addrprefix.ipaddrB8[block] & mask)) {
				return false;
			}
			return true;
		}
	}
	return true;
}

bool compare_ipv6_addr(const ipv6addr& addr, const ipv6addr& addr2)
{
	return (memcmp(addr.ipaddrB8, addr2.ipaddrB8, 16) == 0);
}

bool parse_ipv6_addr(const char* addrstr, ipv6addr& ipaddr)
{
	UInt8 blockIndex = 0;
	char* currentCharPtr = (char*)addrstr;
	while (blockIndex < 8) { // we have 8 blocks
		UInt8 blockLength = 0;

		// Determine block length
		while (*currentCharPtr != ':' && *currentCharPtr != 0) {
			++blockLength;
			++currentCharPtr;
		}
		currentCharPtr -= blockLength; // reset the char pointer
		
		// put digits of the block "132f" into 'blockdata'
		UInt8 blockdata[4] = {0};
		while (blockLength) {
			--blockLength;
			blockdata[blockLength] = numberFromAscii(*currentCharPtr);
			++currentCharPtr;
		}

		// from 'blockdata' digits to the ipv6addr. A block of 4 ascii digits is mapped to
		// 2 ipv6 address bytes.
		ipaddr.ipaddrB8[blockIndex * 2 + 0] = (blockdata[2]) | (blockdata[3] << 4);
		ipaddr.ipaddrB8[blockIndex * 2 + 1] = (blockdata[0]) | (blockdata[1] << 4);

		++blockIndex; // valid block: increase block counter

		if (*currentCharPtr == 0)
			break;
		++currentCharPtr;
	}

	if (blockIndex != 8)
		return 0;
	return 1;
}

void ipv6_addr_toString(const ipv6addr& ipaddr, char* addrstr)
{
	// A block consists of 4 digits and a ":". An IPv6 address has 8 blocks.
	UInt8 addstrPosOffset = 0;
	for (UInt8 block = 0; block < 8; ++block) {
		// first digit of "2001:"
		addrstr[addstrPosOffset + 0] = numberToAscii(ipaddr.ipaddrB8[block * 2 + 0] >> 4); // higher 4 bits
		// second digit of "2001:"
		addrstr[addstrPosOffset + 1] = numberToAscii(ipaddr.ipaddrB8[block * 2 + 0] & 0x0f); // lower 4 bits
		// third digit of "2001:"
		addrstr[addstrPosOffset + 2] = numberToAscii(ipaddr.ipaddrB8[block * 2 + 1] >> 4); // higher 4 bits
		// forth digit of "2001:"
		addrstr[addstrPosOffset + 3] = numberToAscii(ipaddr.ipaddrB8[block * 2 + 1] & 0x0f); // lower 4 bits
		// the colon
		if (block < 7)
			addrstr[addstrPosOffset + 4] = ':';
		addstrPosOffset += 5;
	}
}
}
