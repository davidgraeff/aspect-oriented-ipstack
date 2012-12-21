#include "Management_UDP_Socket.h"
#include "ipstack/udp/UDP.h"

namespace ipstack
{
Management_UDP_Socket Management_UDP_Socket::m_instance;

SendBufferWithInterface* Management_UDP_Socket::requestSendBufferUDP(Interface* interface, unsigned len)
{
	SendBufferWithInterface* b = allocSendBufferWithInterface(estimateSendBufferMinSize() + UDP_Packet::UDP_HEADER_SIZE + len,
								 interface);
	if (!b) return 0;
	prepareSendBuffer(&b->sendbuffer);
	if (!b->sendbuffer.isValid()) {
		mempool->free(b);
		return 0;
	}
	return b;
}
}
