#include "InterfaceAddressesIPv6.h"

namespace ipstack {
	InterfaceAddressesIPv6::InterfaceAddressesIPv6() {
		freeAll();
	}

	AddressEntryIPv6* InterfaceAddressesIPv6::getAddress(AddressPosition* startentry, bool findEmpty) {
		for (AddressPosition i = (startentry ? *startentry : 0); i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			if (bool(memory[i * SIZE]) != findEmpty) {
				if (startentry) *startentry = i + 1;
				return getEntryAtPosition(i);
			}
		}
		return 0;
	}
	
	AddressEntryIPv6* InterfaceAddressesIPv6::getAddressByScope(ipstack::IPV6AddressScope::IPv6_ADDRESS_SCOPE scope) {
		uint8_t nextEntry = 0;
		while (AddressEntryIPv6* entry = getAddress(&nextEntry)) {
			if (IPV6AddressScope::getIPv6AddressScope(entry->ipv6) == scope) {
				return entry;
			}
		}
		return 0;
	}

	void InterfaceAddressesIPv6::freeAll() {
		for (AddressPosition i = 0; i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			memory[i * SIZE] = 0;
		}
	}
	AddressPosition InterfaceAddressesIPv6::getPositionOfAddress(AddressEntryIPv6* entry) {
		for (AddressPosition i = 0; i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			if (getEntryAtPosition(i) == entry)
				return i;
		}
		return EntryUndefined;
	}
	
	AddressEntryIPv6* InterfaceAddressesIPv6::addAddress(const ipstack::ipv6addr& prefix, uint8_t prefixlen,
								uint8_t ipstate = AddressEntryIPv6::AddressEntryIPv6StateTemporary,
								uint8_t routerEntryPosition = AddressMemory::EntryUndefined) {
		AddressEntryIPv6* entry = getAddress(0,true);
		if (!entry)
			return 0;

		copy_ipv6_addr(prefix, entry->ipv6);
		addInterfaceIdentifier(entry->ipv6);
		entry->prefixlen = prefixlen;
		entry->stateflag = 0;
		entry->isOnLink = 1;
		entry->routerEntryPosition = routerEntryPosition;
		entry->state = AddressEntryIPv6::AddressEntryIPv6StateInvalid;
		// Set address state (an address state can be temporary, in this case we have to execute a duplicate address detection)
		setIPv6AddressState(entry, ipstate);
		return entry;
	}

	/**
	* Aspect: If duplicate address detection is configured and
	* the address state is set to AddressEntryIPv6::AddressEntryIPv6StateTemporary
	* do an duplicate address detection.
	* Aspect: If state is invalid and set to temporary, join the respective multicast group.
	*/
	void InterfaceAddressesIPv6::setAddressState(AddressEntryIPv6* entry, uint8_t ipstate) {
		// If not intercepted make this ip valid
		entry->state = AddressEntryIPv6::AddressEntryIPv6StateValid;
	}

	bool InterfaceAddressesIPv6::hasAddress(const ipstack::ipv6addr& addr) {
		uint8_t nextEntry = 0;

		while (AddressEntryIPv6* entry = getAddress(&nextEntry)) {
			if (compare_ipv6_addr(addr, entry->ipv6, entry->prefixlen)) {
				return true;
			}
		}
		return false;
	}

	/**
	* Set the interface up or down. Only an active interface gets a link local address and
	* is able to receive or send IPv6 (inkluding ICMPv6, NDP, MLP) packets.
	* [ASPECT INFLUENCE BY "IPv6_add_link_local_address"]
	*/
	void InterfaceAddressesIPv6::setUp(bool up) {
		if (!up && ipv6_up) { // was online -> take offline
			// free all assigned addresses
			freeAll();
		}
		ipv6_up = up;
	}

	bool InterfaceAddressesIPv6::isUp() {
		return ipv6_up;
	}
}

