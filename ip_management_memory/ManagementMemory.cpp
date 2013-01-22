#include "ManagementMemory.h"
#include "../cfAttribs.h"
#include "SharedMemory.h"
#include "stdio.h"

namespace ipstack
{
	ManagementMemory::ManagementMemory() {
		// Use memory addresses of the global sharedMemory object.
		mempool = &(sharedmemory.mempool);
	}
}
