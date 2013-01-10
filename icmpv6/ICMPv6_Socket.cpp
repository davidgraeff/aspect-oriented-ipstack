#include "ICMPv6_Socket.h"
#include "ICMPv6.h"
#include "ipstack/ipv6/IPv6.h"
#include "util/types.h"

namespace ipstack
{
ICMPv6_Socket ICMPv6_Socket::m_instance;

SendBufferWithInterface* ICMPv6_Socket::requestSendBufferICMP(Interface* interface, UInt16Opt additional_len)
{
	SendBufferWithInterface* b = allocSendBufferWithInterface(estimateSendBufferMinSize() + additional_len, interface);
	if (!b) return 0;
	prepareSendBuffer(&b->sendbuffer);
	if (!b->sendbuffer.isValid()) {
// 		mempool->free(b); // Do not free here: If makeInvalid is called on the sendbuffer it will be collected on the next allocSendBufferWithInterface
		return 0;
	}
	return b;
}
}
