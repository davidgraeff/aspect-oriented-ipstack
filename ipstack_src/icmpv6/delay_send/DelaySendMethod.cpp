#include "DelaySendMethod.h"

#include "ipstack/icmpv6/ICMPv6_Socket.h"

namespace ipstack {
	UInt8 DelaySend::mHoldBack;

	void DelaySend::sendDelayed(SendBuffer* buffer) {
		if (buffer->getState()==SendBuffer::AboutToBeTransmittedState) {
			buffer->setState(SendBuffer::WritingState);
			ICMPv6_Socket::instance().send(buffer);
		}
	}
}