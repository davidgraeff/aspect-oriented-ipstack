Aspect oriented IPstack for embedded systems
============================================
A highly configurable and easily extendable TCP/IP-Stack with IPv4, IPv6, UDP, TCP, ICMPv4, ICMPv6 support.
Abandonment of conditional symbols (#ifdef ...) for configurability...

Work in progress (August 2013):
* Make ipstack independent from CiAO operating system.
* Add linux userspace ipstack support via raw sockets.

Features:
=========
* Dualstack IPv4/IPv6
* UDP
* TCP (Sliding Window, Avoid Silly Window Syndrome, Round-Trip Time Estimation, Congestion Control, MSS, ...)
* ICMPv4 info/error messages
* ICMPv6 info/error messages
* Neigbor discovery protocol (IPv6 - Address resolution, Duplication Check, Router discovery, Autoconfiguration)
* MLD (Multicast Listener Discovery) v1/v2
* ARP (IPv4 - Address resolution protocol)
* Only thin interface to your OS/APP has to be provided, IRQ safe

Integration:
============
* KConfig, Transformation
* ...

Examples:
=========
* On x86 OS (papers, docs, numbers) ...
* On AVR (Complete code) ...

Further reading:
================
