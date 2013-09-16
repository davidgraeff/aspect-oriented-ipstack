# Direct Response
Offers a way to avoid arp/ndp for address resolution but use the link layer remote address directly.
This feature is usefull for ip stack managment tasks like ICMP and tcp half-open request responses,
where an additional address resolution is costly and not necessary.
If this option is not enabled, you have to provide a separate OS Task for management sockets and
copy the received frame to the incoming-buffer of a socket. The socket's task is responsible for
using the known address resolution mechanisms (arp/ndp).

## Technical details
Used by:
* All ICMP functionality that respond directly to a packet
* TCP_Reset

Store a pointer to the beginning of an input stream (link layer data should be located there)
and a boolean "directResponse". Fills in the link layer address of the source as destination address,
before ARP/NDP do a lookup.

# Delay sending
For instance an incoming icmpv6 packet may trigger the generation of multiple response messages, like
an incoming "neighbor solicitation message" have to be answered by an
"neighbor advertisement message" and additionally if the remote host is unknown we have
to send a "neighbor solicitation message" on our own. For the case that the network device has
a shared buffer for incoming/outgoing traffic and because we do not copy the incoming
packet to a buffer of our own in **Direct Response Mode**, it is recommend to activate this option to delay sending
packets until the received packet has been processed entirely. Otherwise it may happen that the incoming
packet will be overwritten by outgoing data.

## Technical details
Add a boolean to the Sendbuffer class to flag a sendbuffer as send-later. If this flag is set and send() is called
we actually send only if the incoming packet processing has been completed, otherwise we do not send but remember
the sendbuffer. If the processing completed, we send all remembered buffers at once.