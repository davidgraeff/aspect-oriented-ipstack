#include "DelaySendMethod.h"

#include "icmpv6/ICMPv6_Socket.h"

namespace ipstack {
	uint8_t DelaySend::mHoldBack;

	void DelaySend::sendDelayed(SendBuffer* buffer) {
		if (buffer->getState()==SendBuffer::AboutToBeTransmittedState) {
			buffer->setState(SendBuffer::WritingState);
			ICMPv6_Socket::instance().send(buffer);
		}
	}
}