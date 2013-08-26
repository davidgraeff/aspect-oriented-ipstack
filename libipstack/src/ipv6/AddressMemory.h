// This file is part of CiAO/IP.
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
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2012 David Gräff

#pragma once

#include "ipv6/IPv6AddressUtilities.h"
#include "util/types.h"
#include "cfAttribs.h"

#include "ndpcache/NDPCacheEntry.h"

namespace ipstack
{
/**
 * An IPv6 address entry. This combines an assigned interface address
 * and a received router prefix. If this is an entry, not assigned by
 * a router but manually, the router_entry_pos is "EntryUndefined".
 *
 * Initially an entry is in the "AddressEntryStateTemporary" state until a
 * neighbour address duplication check has been performed. The entry will either
 * be dismissed (duplicate found) or the state is changed to "AddressEntryStateValid".
 * 
 */
struct AddressEntry {
	enum { ID = 252 };
	unsigned char id;
	
	ipv6addr ipv6;
	UInt8 prefixlen;
	
	enum { AddressEntryStateTemporary = 0, AddressEntryStateValid = 1};
	union {
		UInt8 stateflag;
		struct {
			UInt8 state:2;
			UInt8 isOnLink:1;
			UInt8 toBeRemoved:1;
		} __attribute__((packed));
	};
	
	UInt8 routerEntryPosition;
	UInt8 hoplimit;
	/**
	 * Use this after getting a free memory block from AddressMemory.
	 * This will mark the memory block as beeing used.
	 */
	void reserve() {
		id = ID;
	}
} __attribute__((packed));
#define AddressEntryType AddressEntry, AddressEntry::ID
#define FreeEntryType void, 0

/**
 * A memory block for storing ipv6 address entries (with additional data). This memory
 * is used for three types of entries currently: AddressEntry, NDPCacheEntry, MulticastEntry.
 * Although it is a good idea to store all types in one shared memory instead of having
 * a separate storage for each type (imbalanced usage pattern!), the current implementation
 * is not optimized for efficiency. The memory is divided into segments by the size of the
 * biggest structure that will be stored. If AdressMemory is searched by one type (for example
 * "AddressEntry"-type), every segment in the entire memory have to be looked at.
 *
 * Possible improvement:
 * * Fill memory from two sides. The storage-side depends on the entry type.
 * * Insertion sort: A sorted list of entries (sorting AddressEntry by IPv6, sorting NDPCacheEntry
 *                   by ethernet address) allows to use binary search
 */
class AddressMemory
{
#define maxsize(X, Y)  ((X) > (Y) ? (X) : (Y))
	private:
		enum {SIZE = maxsize(sizeof(AddressEntry), sizeof(NDPCacheEntry)), ENTRIES = cfIPSTACK_IPv6_cache};
		unsigned char memory[SIZE* ENTRIES];
	public:
		enum { EntryUndefined = 255 };
		AddressMemory() {
			// set memory bytes to zero where the id of each entry is located
			for (UInt8 i = 0; i < ENTRIES; ++i) {
				memory[i * SIZE] = 0;
			}
		}

		void freeEntry(void* entry) {
			((char*)entry)[0] = 0;
		}
		/**
		 * The first parameter (if set) is a pointer to the start position
		 * and will return found_position+1.
		 * The second parameter is the type that will be searched for.
		 * type==0: free entry
		 */
		template <class resultType, unsigned char type>
		resultType* findEntry(UInt8* startentry = 0) {
			for (UInt8 i = startentry ? *startentry : 0; i < ENTRIES; ++i) {
				
				if (memory[i * SIZE] == type) {
					if (startentry) *startentry = i + 1;
					return reinterpret_cast<resultType*>(&memory[i * SIZE]);
				}
			}
			return 0;
		}

		void freeAll(unsigned char type) {
			for (UInt8 i = 0; i < ENTRIES; ++i) {
				if (memory[i * SIZE] == type) {
					freeEntry(&memory[i * SIZE]);
				}
			}
		}
		UInt8 getPosition(void* entry) {
			for (UInt8 i = 0; i < ENTRIES; ++i) {
				if (&memory[i * SIZE] == entry) {
					return i;
				}
			}
			return EntryUndefined;
		}
		
		void* getEntryAtPosition(UInt8 i) {
			return &memory[i * SIZE];
		}
};

} //namespace ipstack
