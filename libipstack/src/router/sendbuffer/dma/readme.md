# DMA
If your network device uses DMA (Direct Memory Access), you cannot create a sendbuffer, send it and free it.
The sendbuffer should only be freed if the DMA process has finished. If you enable
**Network Device Uses DMA** feature the `socket->free(..)` method will be intercepted.
Only if `interface->hasBeenSent(SendBuffer*)` returns true the buffer will be freed, if this
is not the case, it will be put in a linked list with all the other not-freed buffers.

## Garbage collection on alloc
If you enabled the feature **Garbage collection before every alloc**,
on every call to `SendBuffer::createInstance` all those buffers will be checked again and freed if possible.

## Block until send
If you enabled the feature **Block until send**,
on every call to `socket->free(..)` the task will be blocked by a busy loop until
`interface->hasBeenSent(SendBuffer*)` returns true.

## Feedback from your hardware
You may use another approach if your hardware provides you with information for finished DMAs.
Use the **No freeing** option. If you for example get a call from your hardware like this:
	void finished_dma(void* memory_address);
Convert the `memory_address` back to a sendbuffer by using the API `Sendbuffer::fromDataStartPointer(void*);`
and call `scoket->freeSendbuffer(sendbuffer)` like this:

	void finished_dma(void* memory_address) {
		Sendbuffer* s = Sendbuffer::fromDataStartPointer(memory_address);
		SocketMemory* socket = s->getSocket();
		socket->freeSendbuffer(s);
	}
Please be aware that the `getSocket()` method is only available with the **No freeing** option.