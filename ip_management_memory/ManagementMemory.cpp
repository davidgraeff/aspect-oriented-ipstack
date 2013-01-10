#include "ManagementMemory.h"
#include "../cfAttribs.h"
#include "SharedMemory.h"
#include "stdio.h"

namespace ipstack
{
	ManagementMemory::ManagementMemory() {
		// Use memory addresses of the global sharedMemory object.
		mempool = &(sharedmemory.mempool);
		packetbuffer = &(sharedmemory.buf);
	}
	SendBufferWithInterface* ManagementMemory::allocSendBufferWithInterface(UInt16Opt size, Interface* interface) {
		freeAllUnused();
		SendBufferWithInterface* sbi = SendBufferWithInterface::createSendBuffer(mempool, size, interface);
		if (sbi)
			packetbuffer->put(sbi);
		return sbi;
	}
	void ManagementMemory::freeAllUnused() {
		for (UInt16Opt i = 0; i < sharedmemory.getSlots(); ++i) {
			SendBufferWithInterface* old = (SendBufferWithInterface*)packetbuffer->get();
			if (!old) {
				break;
			} else if (old->hasBeenSend()) { // if the SendBuffer has been send already, free it
				mempool->free((void*)old);
			} else {
				// not send, reinsert
				packetbuffer->put(old);
			}
		}
	}
	
	const UInt16Opt ManagementMemory::getSlots() const {
		return sharedmemory.getSlots();
	}
}
