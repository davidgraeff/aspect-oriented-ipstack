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
#include "IPv6_Config.h"

#include "util/ipstack_inttypes.h"

namespace ipstack
{
/**
 * An IPv6 assigned interface address prefix. If this is an entry, not assigned by
 * a router but manually, the routerEntryPosition is "EntryUndefined".
 *
 * Initially an entry is in the "AddressEntryStateTemporary" state until a
 * neighbour address duplication check has been performed. The entry will either
 * be dismissed (duplicate found) or the state is changed to "AddressEntryStateValid".
 */
struct AddressEntry {
	uint8_t prefixlen; // 0 means entry is not used!
	
	enum { AddressEntryStateTemporary = 0, AddressEntryStateValid = 1};
	union {
		uint8_t stateflag;
		struct {
			uint8_t state:2;
			uint8_t isOnLink:1;
			uint8_t toBeRemoved:1;
		} __attribute__((packed));
	};
	
	uint8_t routerEntryPosition;
	uint8_t hoplimit;
	
	ipv6addr ipv6;
} __attribute__((packed));

/**
 * A memory block for storing ipv6 address entries
 * Possible improvement:
 * Insertion sort: A sorted list of entries (sorting AddressEntry by IPv6) allows to use binary search.
 * Because we assume to have only 2-3 ipv6 prefixes in average, the current implementation stores the
 * prefixes unordered. 
 */
class InterfacePrefixes
{
	private:
		// Define SIZE
		enum {SIZE = sizeof(AddressEntry)};
		typedef uint8_t AddressPosition;
		
		unsigned char memory[SIZE* IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE];
	public:
		enum { EntryUndefined = 255 };
		InterfacePrefixes();
		inline void freeEntry(void* entry) { ((char*)entry)[0] = 0; }
		
		inline AddressEntry* getEntryAtPosition(AddressPosition i)
		{ return reinterpret_cast<AddressEntry*>(&memory[i * SIZE]); }

		/**
		 * The first parameter (if set) is a pointer to the start position
		 * and will return found_position+1.
		 * The second parameter is the type that will be searched for.
		 * type==0: free entry
		 */
		AddressEntry* findEntry(AddressPosition* startentry = 0, bool findEmpty = false);

		void freeAll();
		
		AddressPosition getPosition(AddressEntry* entry);
};

} //namespace ipstack
