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
// Copyright (C) 2013 David Gräff

#pragma once


#include "util/ipstack_inttypes.h"
#include "util/singleton.h"
#include "NeighbourEntry.h"

namespace ipstack
{

/**
 * API declaration for a neighbour cache
 */
class NeighbourCache : public Singleton<NeighbourCache>
{
	private:
		// Define SIZE
		enum {SIZE = sizeof(NeighbourEntry)};
		
		unsigned char memory[SIZE* NEIGHBOURCACHE_ENTRIES];
	public:
		typedef uint8_t EntryPosition;
		enum { EntryUndefined = ((EntryPosition)-1), EntryMulticast = ((EntryPosition)-2) };
		NeighbourCache();
		void freeEntry(void* entry);
		
		// For aspects to react to removed routers
		void removedRouter(NeighbourEntry*, EntryPosition position) {}
		void removedNonRouter(NeighbourEntry*, EntryPosition position) {}
		
		inline NeighbourEntry* getEntryAtPosition(EntryPosition i)
		{ return reinterpret_cast<NeighbourEntry*>(&memory[i * SIZE]); }

		inline bool is_reachable(NDPCacheEntry* entry) {
			return (entry->state == NDPCacheEntry::NDPCacheEntryState_Reachable);
		}

		/**
		* If an entry has been confirmed by external means (static or by NDP Neighbor Advertisement)
		* this method is called. This can be used by aspects to for example reset the expire time.
		* The basic implementation does nothing.
		*/
		void entryConfirmed(NDPCacheEntry* entry) {}
		
		/**
		* An aspect has to weave in a strategy to regain space in the NDP cache.
		*/
		void dropUnused() {}
		
		/**
		 * The first parameter (if set) is a pointer to the start position
		 * and will return found_position+1.
		 * The second parameter is the type that will be searched for.
		 * type==0: free entry
		 */
		NeighbourEntry* findEntry(EntryPosition* startentry = 0, bool findEmpty = false);

		void freeAll();
		
		EntryPosition getPosition(NeighbourEntry* entry);
	private:
		NeighbourCache() {} //ctor hidden
		NeighbourCache(const NeighbourCache&); //copy ctor hidden
		NeighbourCache& operator=(NeighbourCache const&); // assign op. hidden
};

} //namespace ipstack
