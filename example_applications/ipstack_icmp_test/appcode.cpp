#include "appcode.h"
#include "as/AS.h"
#include "ipstack/as/EventSupport.h"
#include "ipstack/ethernet/EthernetAddressUtilities.h"
#include "ipstack/icmpv4/ICMPv4_Socket.h"
#include "ipstack/icmpv6/ICMPv6_Socket.h"
#include "ipstack/icmpv4/ICMP.h"
#include "ipstack/icmpv6/ICMPv6.h"
#include "ipstack/ipv6/udp/ReceiveBufferUDPIPv6.h"
#include "ipstack/ipv4/udp/ReceiveBufferUDPIPv4.h"
#include "objects.h"
#include "stdio.h"
#include "ipstack/as/SystemHalt.h"

#include "ipstack/api/Setup.h"
#include "ipstack/api/UDP_Socket.h"

/*** ---------------------------------------------------------*/
ipstack::Interface* setupEthernet() {
	ipstack::Interface* interface = IP::getInterface(0);
	if (!interface) {
		printf("Setting up networking... failed\n");
	} else {
		printf("Setting up network for device #0: %s\n", interface->getName() );

		printf("  MAC-Address: ");
		const UInt8* hw_addr = interface->getAddress();
		for(int i=0; i<5; ++i){
		printf("%X:", hw_addr[i]);
		}
		printf("%X\n", hw_addr[5]);
		
		printf("  MTU: %u\n", interface->getMTU());

		interface->setIPv4Addr(10,0,3,2);
		printf("  IPv4-Address: 10.0.3.2\n");

		interface->setIPv4Subnetmask(255,255,255,0);
		printf("  IPv4-Subnetmask: 255.255.255.0\n");

		interface->setIPv4Up(true);
		printf("  IPv4-Status: Up (Running)\n");


		interface->setIPv6Up(true);
		printf("  IPv6-Status: Up (Running)\n");
		UInt8 nextEntry = 0;
		while (AddressEntry* entry = interface->addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
			char buffer[50] = {0};
			ipstack::ipv6_addr_toString(entry->ipv6, buffer);
			printf("  IPv6-Address: %s\n");
		}
	}
	return interface;
} // *** ---------------------------------------------------------

void Alpha::functionTaskTask0() {
	ipstack::Interface* interface = setupEthernet();
			
	if (interface==0) {
			// ethernet setup failed
			System::haltsystem();
	}

	// You can ping CiAO IP now by using
	// * ping 10.0.3.2 (for icmp ping)
	// * ping6 fe80::6655:44ff:fe33:2211%tap0 (for icmp ping with ipv6 on tap0 device)
	// * echoping -u 10.0.3.2 (for udp ping; use ipv4 addresses, ipv6 is not supported by echoping for udp)
	// * sendip -p ipv6 -p udp -us 5070 -ud 8 -d "Hello" -v fe80::6655:44ff:fe33:2211 (for ipv6 udp ping)
	// * telnet 10.0.3.2 (for testing the tcp reset capability; use ipv4 or ipv6 address)
	// * Send udp to port 88 to get it printed out
	
	// Setup test ipv6 address: we will ping that address (and test the ndp address resolution)
	ipstack::ipv6addr testipv6;
	if (!parse_ipv6_addr("fe80:0:0:0:a81a:54ff:fef0:3f", testipv6)) {
		System::haltsystem();
	}
	ICMPv6_Socket &icmpv6instance = ICMPv6_Socket::instance();
	icmpv6instance.ipv6.set_dst_addr(testipv6); // src address is determined automatically

	// Setup test ipv4 address: we will ping that address (and test the arp address resolution)
	UInt32 testipv4 = IPv4_Packet::convert_ipv4_addr(10,0,3,1);
	
	ICMPv4_Socket &icmpv4instance = ICMPv4_Socket::instance();
	icmpv4instance.ipv4.set_dst_addr(testipv4); // src address has been set above

	SendBuffer* sbi;
	const char teststring[] = "HELLO";
	
	
	IP::UDP_Socket<1514,1,128,2> socket;
	socket.set_sport(88);
	socket.bind(); // listen
  
	printf("Send IPv4 ping\n");
	sbi = icmpv4instance.requestSendBuffer(ICMP_Packet::ICMP_HEADER_SIZE+sizeof(teststring));
	if (sbi) {
		sbi->mark("IPv4 ping");
		ICMP_Packet* icmp = (ICMP_Packet*)sbi->getDataPointer();

		icmp->set_type(ICMP_Packet::ICMP_TYPE_ECHO_REQUEST);
		icmp->set_code(ICMP_Packet::ICMP_CODE_ECHO);

		sbi->writtenToDataPointer(ICMP_Packet::ICMP_HEADER_SIZE);
		sbi->write(teststring, sizeof(teststring));

		icmpv4instance.send(sbi);
	}

	printf("Send IPv6 ping\n");
	sbi = icmpv6instance.requestSendBuffer(ICMPv6_Packet::ICMP_HEADER_SIZE+sizeof(teststring)+sizeof(UInt32));
	if (sbi) {
		sbi->mark("IPv6 ping");
		ICMPv6_Packet* icmp = (ICMPv6_Packet*)sbi->getDataPointer();

		icmp->set_type(ICMPv6_Packet::ICMP_TYPE_ECHO_REQUEST);
		icmp->set_code(ICMPv6_Packet::ICMP_CODE_ECHO_REQUEST);

		sbi->writtenToDataPointer(ICMPv6_Packet::ICMP_HEADER_SIZE);
		// write seq and id
		UInt32 idAndSeq = 0xcaffee;
		sbi->write(&idAndSeq, sizeof(idAndSeq));
		// write payload
		sbi->write(teststring, sizeof(teststring));

		icmpv6instance.send(sbi);
	}
	
	printf("Start udp listen on port %u\n", socket.get_sport());
	while(1) {
		ReceiveBuffer* rbuffer = socket.receiveBlock();

		ReceiveBufferUDPIPv6* ipv6udp = ReceiveBufferUDPIPv6::cast(rbuffer);
		ReceiveBufferUDPIPv4* ipv4udp = ReceiveBufferUDPIPv4::cast(rbuffer);
		if (ipv6udp) {
			printf("Received IPv6/UDP. Bytes: %u\n", rbuffer->getSize());
			char ipstrbuffer[50] = {0};
			ipstack::ipv6_addr_toString(ipv6udp->getRemoteInfo()->ipv6, ipstrbuffer);
			printf("  Remote IPv6-Address: %s, Port: %u\n", ipstrbuffer, ipv4udp->getRemoteInfo()->remoteport);
		} else if (ipv4udp) {
			printf("Received IPv4/UDP. Bytes: %u\n", rbuffer->getSize());
			UInt8 a,b,c,d;
			IPv4_Packet::convert_ipv4_addr(ipv4udp->getRemoteInfo()->ipv4, a, b, c, d);
			printf("  Remote IPv4-Address: %u.%u.%u.%u, Port: %u\n", a,b,c,d, ipv4udp->getRemoteInfo()->remoteport);
		} else {
			printf("error, udp ip version unknown. Bytes: %u\n", rbuffer->getSize());
		}
		ReceiveBuffer::free(rbuffer);
	}
}

