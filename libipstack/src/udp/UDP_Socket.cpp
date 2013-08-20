#include "UDP.h"
#include "UDP_Socket.h"

namespace ipstack {
void UDP_Socket::setupHeader(ipstack::UDP_Packet* packet, unsigned int datasize)
{
	packet->set_dport(dport);
	packet->set_sport(sport);
	packet->set_length(datasize); // incl. UDP_Packet::UDP_HEADER_SIZE!
	packet->set_checksum(0);
}
UDP_Socket::UDP_Socket() : mempool(0), dport(UDP_Packet::UNUSED_PORT),
	sport(UDP_Packet::UNUSED_PORT) {}
void UDP_Socket::set_Mempool(ipstack::Mempool* m)
{
	mempool = m;
}
ipstack::Mempool* UDP_Socket::get_Mempool()
{
	return mempool;
}
void UDP_Socket::set_ReceiveQueue(ipstack::Packetbuffer* buf)
{
	m_receivequeue = buf;
}
}
