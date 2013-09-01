#include "udp/UDP_Packet.h"
#include "udp/UDP_Socket.h"

namespace ipstack {
	UDP_Socket::UDP_Socket(const SocketMemory& memory) :
		SocketMemory(memory), dport(UDP_Packet::UNUSED_PORT),	sport(UDP_Packet::UNUSED_PORT) {}

	UDP_Socket::~UDP_Socket() {}
	
	void UDP_Socket::setupHeader(ipstack::UDP_Packet* packet, unsigned int datasize)
	{
		packet->set_dport(dport);
		packet->set_sport(sport);
		packet->set_length(datasize); // incl. UDP_Packet::UDP_HEADER_SIZE!
		packet->set_checksum(0);
	}

	void UDP_Socket::set_dport(uint16_t d) {
		dport = d;
	}
	uint16_t UDP_Socket::get_dport() {
		return dport;
	}

	/// Source port
	void UDP_Socket::set_sport(uint16_t s) {
		sport = s;
	}
	uint16_t UDP_Socket::get_sport() {
		return sport;
	}

	bool UDP_Socket::bind() {return false;}
	void UDP_Socket::unbind() {}
}