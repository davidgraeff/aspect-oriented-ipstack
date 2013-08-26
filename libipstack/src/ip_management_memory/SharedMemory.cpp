#include "SharedMemory.h"
#include "SendBuffer.h"
#include "cfAttribs.h"

namespace ipstack
{
	SharedMemory sharedmemory;
	
	SharedMemory::SharedMemory() {
		memset(allocatedMemories, 0, SLOTS*sizeof(void*));
	}

	bool SharedMemory::insert(SendBuffer* data) {
		for (UInt16Opt i=0;i<SLOTS;++i) {
			if (!allocatedMemories[i]) {
				allocatedMemories[i] = data;
				return true;
			}
		}
		return false;
	}
	
	bool SharedMemory::remove(UInt16Opt index) {
		SendBuffer* buffer = allocatedMemories[index];
		UInt8 s = buffer->getState();
		if ( s == SendBuffer::InvalidState || (s == SendBuffer::TransmittedState && buffer->hasBeenSend())) { // if the SendBuffer has been send already, free it
			mempool.free((void*)buffer);
			allocatedMemories[index] = 0;
			return true;
		}
		return false;
	}
	
	SendBuffer* SharedMemory::getNext(UInt16Opt& index) const {
		for (UInt16Opt i=index;i<SLOTS;++i) {
			if (allocatedMemories[i]) {
				index = i;
				return allocatedMemories[i];
			}
		}
		return 0;
	}
	
	void SharedMemory::freeAllUnused() {
		for (UInt16Opt i=0;i<SLOTS;++i) {
			if (allocatedMemories[i]) {
				remove(i);
			}
		}
	}
}
