#include "ICMPv4_Socket.h"
#include "ICMP.h"
#include "util/types.h"
#include "stdio.h"

namespace ipstack
{
ICMPv4_Socket ICMPv4_Socket::m_instance;

SendBufferWithInterface* ICMPv4_Socket::requestSendBufferICMP(Interface* interface, UInt16Opt additional_len)
{
	SendBufferWithInterface* b = allocSendBufferWithInterface(estimateSendBufferMinSize() +
								 ICMP_Packet::ICMP_HEADER_SIZE + additional_len,
								 interface);
	if (!b) return 0;

	prepareSendBuffer(&(b->sendbuffer));
	if (!b->sendbuffer.isValid()) {
		mempool->free(b);
		return 0;
	}
	return b;
}
}
