#include "ethernet/EthernetAddressUtilities.h"
#include "icmpv4/ICMPv4_Socket.h"
#include "icmpv6/ICMPv6_Socket.h"
#include "icmpv4/ICMPv4_Packet.h"
#include "icmpv6/ICMPv6_Packet.h"
#include "ipv6/udp/ReceiveBufferUDPIPv6.h"
#include "ipv4/udp/ReceiveBufferUDPIPv4.h"
#include "stdio.h"

#include "api/Setup.h"
#include "api/OS_Integration.h"
#include "udp/UDP_Socket.h"
#include "../common/setupFirstInterface.h"
#include "memory_management/Mempool_Instance_TCP.h"

void example_main() {
	setupFirstInterface<true, true>(); // enable ipv4/v6
	
	SendBuffer* sbi;
	const char teststring[] = "HELLO";

	{ // IPv4 ping
		
		RawIP_Socket socket;
		// Setup test ipv4 address: we will ping that address (and test the arp address resolution)
		icmpv4instance.ipv4.set_dst_addr(IPv4_Packet::convert_ipv4_addr(10,0,3,1));

		printf("Send IPv4 ping\n");
		sbi = icmpv4instance.requestSendBuffer(ICMP_Packet::ICMP_HEADER_SIZE+sizeof(teststring));
		if (sbi) {
			ICMP_Packet* icmp = (ICMP_Packet*)sbi->getDataPointer();

			icmp->set_type(ICMP_Packet::ICMP_TYPE_ECHO_REQUEST);
			icmp->set_code(ICMP_Packet::ICMP_CODE_ECHO);

			sbi->writtenToDataPointer(ICMP_Packet::ICMP_HEADER_SIZE);
			sbi->write(teststring, sizeof(teststring));

			icmpv4instance.send(sbi);
		}
	}

	{ // IPv6 ping
		// Setup test ipv6 address: we will ping that address (and test the ndp address resolution)
		ipstack::ipv6addr testipv6;
		if (!parse_ipv6_addr("fe80:0:0:0:a81a:54ff:fef0:3f", testipv6)) {
			System::haltsystem();
		}
		ICMPv6_Socket &icmpv6instance = ICMPv6_Socket::instance();
		icmpv6instance.ipv6.set_dst_addr(testipv6); // src address is determined automatically

		printf("Send IPv6 ping\n");
		sbi = icmpv6instance.requestSendBuffer(ICMPv6_Packet::ICMP_HEADER_SIZE+sizeof(teststring)+sizeof(UInt32));
		if (sbi) {
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
	}
	
	IP::UDP_Socket<1514,1,128,2> udpsocket;
	udpsocket.set_sport(88);
	udpsocket.bind(); // listen
	
	printf("Start udp listen on port %u\n", udpsocket.get_sport());
	while(1) {
		SmartReceiveBufferPtr rbuffer = udpsocket.receiveBlock();
		const uint8_t version = IP::get_ip_version(rbuffer->p.ip_packet);
		UDP_Packet* udp_packet = (UDP_Packet*)rbuffer->p.transport_packet;
		
		printf("Received UDP. Bytes: %u, Port: %u\n", rbuffer->get_payload_size(), udp_packet->get_sport());
		if (version==6) {
			IPv6_Packet* ip_packet = (IPv6_Packet*)rbuffer->p.ip_packet;
			char ipstrbuffer[50] = {0};
			ipstack::ipv6_addr_toString(ip_packet->get_src_ipaddr(), ipstrbuffer);
			printf("  IPv6-Address: %s\n", ipstrbuffer);
			
		} else if (version==4) {
			IPv4_Packet* ip_packet = (IPv4_Packet*)rbuffer->p.ip_packet;
			uint8_t a,b,c,d;
			IPv4_Packet::convert_ipv4_addr(ip_packet->get_src_ipaddr(), a, b, c, d);
			printf("  IPv4-Address: %u.%u.%u.%u\n", a,b,c,d);
		}
	}
}

