#include "ICMPv4_Socket.h"
#include "ICMP.h"
#include "util/types.h"
#include "stdio.h"

namespace ipstack
{
ICMPv4_Socket ICMPv4_Socket::m_instance;

SendBufferWithInterface* ICMPv4_Socket::requestSendBufferICMP(Interface* interface, UInt16Opt additional_len)
{
	SendBufferWithInterface* b = allocSendBufferWithInterface(estimateSendBufferMinSize() + additional_len, interface);
	if (!b) return 0;

	prepareSendBuffer(&(b->sendbuffer));
	if (!b->sendbuffer.isValid()) {
// 		mempool->free(b); // Do not free here: If makeInvalid is called on the sendbuffer it will be collected on the next allocSendBufferWithInterface
		return 0;
	}
	return b;
}
}
