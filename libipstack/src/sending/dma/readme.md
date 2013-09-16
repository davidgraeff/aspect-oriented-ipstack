# DMA
If your network device uses dma, you cannot create a sendbuffer, send it and free it.
The sendbuffer should only be freed if the DMA process has finished. If you enable the
**Garbage collection before every alloc** feature, the `socket->free(..)` method will be intercepted.
Only if `interface->hasBeenSend(SendBuffer*)` returns true the buffer will be freed, if this
is not the case, it will be put in a linked list with all the other not-freed buffers.
On every call to `alloc` all those buffers will be checked again and freed if possible.

You may use another approach if your hardware provides you with information for finished
DMAs.
