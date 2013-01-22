#include "ManagementMemory.h"
#include "SharedMemory.h"

namespace ipstack
{
	ManagementMemory::ManagementMemory() {
		// Use memory addresses of the global sharedMemory object.
		mempool = &(sharedmemory.mempool);
	}
}
