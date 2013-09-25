# DMA
If your network device uses DMA (Direct Memory Access), you cannot create a sendbuffer, send it and free it.
The sendbuffer should only be freed if the DMA process has finished. If you enable
**Network Device Uses DMA** feature the `socket->free(..)` method will be intercepted.
Only if `interface->hasBeenSend(SendBuffer*)` returns true the buffer will be freed, if this
is not the case, it will be put in a linked list with all the other not-freed buffers.

## Garbage collection on alloc
If you enabled the feature **Garbage collection before every alloc**,
on every call to `SendBuffer::createInstance` all those buffers will be checked again and freed if possible.

## Block until send
If you enabled the feature **Block until send**,
on every call to `socket->free(..)` the task will be blocked by a busy loop until
`interface->hasBeenSend(SendBuffer*)` returns true.

## Feedback from your hardware
You may use another approach if your hardware provides you with information for finished DMAs.