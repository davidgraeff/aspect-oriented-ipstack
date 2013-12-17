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

#include "util/ipstack_inttypes.h"
#include "router/Interface.h"

#include "ipv6/sending/RouteResult.h"
#include "ipv6/IPv6AddressUtilities.h"
#include "IPv6_Config.h"

#include "ip/InterfaceAddressEntry.h"
#include "util/ipstack_inttypes.h"

namespace ipstack {

class AddressEntryIPv6 : public AddressEntry {
public:
	ipv6addr ipv6;
} __attribute__((packed));


/**
 * A memory block for storing ipv6 address entries
 * Possible improvement:
 * Insertion sort: A sorted list of entries (sorting AddressEntryIPv6 by IPv6) allows to use binary search.
 * Because we assume to have only 2-3 ipv6 prefixes in average, the current implementation stores the
 * prefixes unordered. 
 */
class InterfaceAddressesIPv6 {
	public:
		typedef uint8_t AddressPosition;
		enum { EntryUndefined = 255 };
		InterfacePrefixes();
		
		/// Free entries
		inline void freeAddress(void* entry) { ((char*)entry)[0] = 0; }
		void freeAll();
		
		/// Get entries
		inline AddressEntryIPv6* getAddressAtPosition(AddressPosition i)
		{ return reinterpret_cast<AddressEntryIPv6*>(&memory[i * SIZE]); }
		AddressPosition getPositionOfAddress(AddressEntryIPv6* entry);

		/**
		 * The first parameter (if set) is a pointer to the start position
		 * and will return found_position+1.
		 * The second parameter is the type that will be searched for.
		 * type==0: free entry
		 */
		AddressEntryIPv6* getAddress(AddressPosition* startentry = 0, bool findEmpty = false);

		/**
		* Return true if an IPv6 address is bound to this interface that machtes the given scope (link-local, global unique, etc).
		* The first matched address is returned by the first parameter "addr". getInterfaceIPv6AddressByScope
		*/
		AddressEntryIPv6* getAddressByScope(ipstack::IPV6AddressScope::IPv6_ADDRESS_SCOPE scope);

		/**
		* Make an IPv6 address out of a prefix and add it to the interface.
		* Return true if an IPv6 address could be added to the IP-Addresses of this interface.
		* @arg prefix The IPv6 address prefix that should be bound to this interface
		* @arg prefixlen If this is an IP derived from
		*
		* If this entry belongs to a router, set the pointerToRouterEntry to the ndp_cache position.
		*/
		AddressEntryIPv6* addAddress(const ipstack::ipv6addr& prefix, uint8_t prefixlen,
									uint8_t ipstate = AddressEntryIPv6::AddressEntryIPv6StateTemporary,
									uint8_t routerEntryPosition = AddressMemory::EntryUndefined);
		
		/**
		* Aspect: If duplicate address detection is configured and
		* the address state is set to AddressEntryIPv6::AddressEntryIPv6StateTemporary
		* do an duplicate address detection.
		*/
		void setAddressState(AddressEntryIPv6* entry, uint8_t ipstate);

		bool hasAddress(const ipstack::ipv6addr& addr);

		/**
		* Set the interface up or down. Only an active interface gets a link local address and
		* is able to receive or send IPv6 (inkluding ICMPv6, NDP, MLP) packets.
		* [ASPECT INFLUENCE BY "IPv6_add_link_local_address"]
		*/
		void setUp(bool up);
		bool isUp();
	private:
		// Define SIZE
		enum {SIZE = sizeof(AddressEntryIPv6)};
		unsigned char memory[SIZE* IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE];
		bool ipv6_up;
		
		/**
		* Generate a full valid IPv6 address by providing an IPv6-Prefix
		* (there may be duplicates on-link!)
		* Input: address prefix (e.g. local-link, multicast id, global-unique-id)
		* Output: IP with interface identifier (e.g. ethernet mac).
		*/
		void addInterfaceIdentifier(ipstack::ipv6addr& address) { /*[ASPECT WEAVE IN]*/ }

};

} //namespace ipstack
