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
// Copyright (C) 2011 Christoph Borchert


#include "IPv4ToLinkLayer.h"
#include "NeighbourCacheConfig.h"

namespace ipstack {
	uint8_t* AddressResolutionIPv4::find(uint32_t ipv4_addr){
		for(unsigned i=0; i<MAX_CACHE_ENTRIES; i++){      
			cache_entry& entry = entries[i];
			if(entry.ipv4_addr == ipv4_addr){
			return entry.hw_addr;
			}
		}
		return 0;
	}

	// handle incoming arp replies: update own cache
	void AddressResolutionIPv4::update(const uint32_t ipv4_addr, const uint8_t* hw_addr){
		unsigned selected_index = AddressResolutionIPv4::MAX_CACHE_ENTRIES;
		unsigned first_free_index = AddressResolutionIPv4::MAX_CACHE_ENTRIES;
		
		for(unsigned i=0; i<AddressResolutionIPv4::MAX_CACHE_ENTRIES; i++){      
		AddressResolutionIPv4::cache_entry& entry = entries[i];
		if(entry.ipv4_addr == ipv4_addr){
			selected_index = i;
			break;
		}
		else if(entry.ipv4_addr == 0){
			if(first_free_index == AddressResolutionIPv4::MAX_CACHE_ENTRIES){
			first_free_index = i;
			}
		}
		}
		if(selected_index == AddressResolutionIPv4::MAX_CACHE_ENTRIES){
		//ipv4_addr is not in cache table yet
		if(first_free_index != AddressResolutionIPv4::MAX_CACHE_ENTRIES){
			//there is a free cache slot left
			selected_index = first_free_index;
		}
		else{
			//evict cache entry
			//TODO
			selected_index = 0; //implement strategy
		}
		}

		AddressResolutionIPv4::cache_entry& entry = entries[selected_index];
		entry.ipv4_addr = ipv4_addr;
		memcpy(entry.hw_addr, hw_addr, 6);
	}
} // namespace ipstack
