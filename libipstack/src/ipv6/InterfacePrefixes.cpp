#include "InterfacePrefixes.h"

namespace ipstack {
	InterfacePrefixes::InterfacePrefixes() {
		freeAll();
	}

	AddressEntry* InterfacePrefixes::findEntry(AddressPosition* startentry, bool findEmpty) {
		for (AddressPosition i = (startentry ? *startentry : 0); i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			if (bool(memory[i * SIZE]) != findEmpty) {
				if (startentry) *startentry = i + 1;
				return getEntryAtPosition(i);
			}
		}
		return 0;
	}

	void InterfacePrefixes::freeAll() {
		for (AddressPosition i = 0; i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			memory[i * SIZE] = 0;
		}
	}
	AddressPosition InterfacePrefixes::getPosition(AddressEntry* entry) {
		for (AddressPosition i = 0; i < IPSTACK_IPV6_INTERFACE_PREFIXES_SIZE; ++i) {
			if (getEntryAtPosition(i) == entry)
				return i;
		}
		return EntryUndefined;
	}
}

