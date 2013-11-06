# Direcory "ip"
This directory contains common IP related code, not specific to any IP version or very specific for
IP versions (with implementations for every version).
You'll find the single/dual-stack implementation in the subdirectory **dual_single_stack**.

## Checksums
There is common checksumming code, ipv4 and ipv6 code and checksum offloading

## IP Socket
Provides you an IP socket. You can send and receive raw IP packets. ICMPv4 and ICMPv6,
for example, use an IP socket as base class.

## Neighbour Cache
Caches IP to link layer addresses. Support for a link layer is added elsewhere. For ethernet,
for instance, look in the "ethernet/" directory.

## Management Task
The IP management task class is a central place for managing memory for IP related functionallity
like ICMPv4/v6 and IPStack services like UDP echo. The class, implemented as Singleton, is also
responsible to provide access to the ICMPv4/v6 etc sockets.

Ideally your operating system supports multi tasking and executes the management task object
in a separate task. If this is not the case, icmp, arp etc are processed by the
**ipstack::System::periodic()** method.