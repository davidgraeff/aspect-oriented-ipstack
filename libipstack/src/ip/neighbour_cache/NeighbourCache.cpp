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

#include "NeighbourCache.h"

namespace ipstack {
	NeighbourCache::NeighbourCache() {
		freeAll();
	}
	
	void NeighbourCache::freeEntry(void* entry) {
		EntryPosition pos = getPosition(entry);
		if (pos == EntryUndefined)
			return;
		if (entry->isRouter) {
			removedRouter(entry);
		} else
			removedNonRouter(entry);
		((char*)entry)[0] = 0;
	}

	NeighbourEntry* NeighbourCache::findEntry(EntryPosition* startentry, bool findEmpty) {
		for (EntryPosition i = (startentry ? *startentry : 0); i < NEIGHBOURCACHE_ENTRIES; ++i) {
			if (bool(memory[i * SIZE]) != findEmpty) {
				if (startentry) *startentry = i + 1;
				return getEntryAtPosition(i);
			}
		}
		return 0;
	}

	void NeighbourCache::freeAll() {
		for (EntryPosition i = 0; i < NEIGHBOURCACHE_ENTRIES; ++i) {
			memory[i * SIZE] = 0;
		}
	}
	EntryPosition NeighbourCache::getPosition(NeighbourEntry* entry) {
		for (EntryPosition i = 0; i < NEIGHBOURCACHE_ENTRIES; ++i) {
			if (getEntryAtPosition(i) == entry)
				return i;
		}
		return EntryUndefined;
	}
} //namespace ipstack
