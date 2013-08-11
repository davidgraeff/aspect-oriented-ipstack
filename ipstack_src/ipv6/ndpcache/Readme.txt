This folder contains the Neigbor Discovery Protocol Cache implementation.
NDP Messages are used to get a link layer address for a given IP address.
This is analogue to the arp cache for IPv4.

In contrast to the arp cache implementation, the ndp cache implementation is tied to an
interface instead of beeing global. W.l.o.g. this can be done due to every entry would have
to store the interface additionally in a global implementation.

Because NDP is link layer agnostic your link layer have to provide some functionality. 
If no link layer is selected or the choosen link layer do not support the NDP Cache compiling will fail.

Storage:
NDP Cache entries are stored in the addressmemory that is used to store
interface IPv6 addresses. This is done to preserve memory by not using two distict
memory blocks and to take advantage of the fact by different usage scenarious
either the NDP Cache is usued heavily or the interface IPv6 address storage.

Synchronous:
Without further influence by an aspect this implementation is synchronous:
The process flow is blocked while looking up a link layer address.

Entry Expire:
Without further influences by aspects an ndp cache entry may never expire.
