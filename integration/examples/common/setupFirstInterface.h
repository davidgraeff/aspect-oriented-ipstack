#pragma once 
#include "api/Setup.h"
#include "api/OS_Integration.h"

template< bool enabled >
class setupIPv4 {
public:
    static inline void setupIPv4(ipstack::Interface* interface){
		interface->setIPv4Addr(10,0,3,2);
		printf("  IPv4-Address: 10.0.3.2\n");

		interface->setIPv4Subnetmask(255,255,255,0);
		printf("  IPv4-Subnetmask: 255.255.255.0\n");

		interface->setIPv4Up(true);
		printf("  IPv4-Status: Up (Running)\n");
    }
};

class setupIPv4< false > {
public:
    static inline void setupIPv4(ipstack::Interface* interface){}
};

template< bool enabled >
class setupIPv6 {
public:
    static inline void setupIPv6(ipstack::Interface* interface){
		interface->setIPv6Up(true);
		printf("  IPv6-Status: Up (Running)\n");
		UInt8 nextEntry = 0;
		while (AddressEntry* entry = interface->addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
			char buffer[50] = {0};
			ipstack::ipv6_addr_toString(entry->ipv6, buffer);
			printf("  IPv6-Address: %s\n", buffer);
		}
    }
};

class setupIPv6< false > {
public:
    static inline void setupIPv6(ipstack::Interface* interface){}
};

inline void printEthernetData(ipstack::Interface* interface) {
	printf("Setting up network for device #0: %s\n", interface->getName() );

	printf("  MAC-Address: ");
	const UInt8* hw_addr = interface->getAddress();
	for(int i=0; i<5; ++i){
	printf("%X:", hw_addr[i]);
	}
	printf("%X\n", hw_addr[5]);
	
	printf("  MTU: %u\n", interface->getMTU());
}

template<bool ipv4, bool ipv6>
ipstack::Interface* setupFirstInterface(bool block_on_error = true) {
	ipstack::Interface* interface = IP::getInterface(0);
	if (interface) {
		printEthernetData(interface);
		setupIPv4::setupIPv4<ipv4>(interface);
		setupIPv6::setupIPv6<ipv6>(interface);
	}
	
	if (block_on_error && !interface) {
		// ethernet setup failed
		printf("No ethernet card found!\n");
		System::haltsystem();
	}
	return interface;
}
