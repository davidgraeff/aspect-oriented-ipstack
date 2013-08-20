Aspect oriented IPstack for embedded systems
============================================
A highly configurable and easily extendable TCP/IP-Stack with IPv4, IPv6, UDP, TCP, ICMPv4, ICMPv6 support.
This network stack is written in AspectC++ and because it is build around the idea of aspects you
may select your desired functionallity during build time in a very fine grained way.

You may use this software in different ways.
There are two examples of integrating the IP-Stack into operating systems for embedded systems
and another example of using it as a linux userspace application on a tun-device.

If you already use the CMake buildsystem, integrating the IP-Stack is very easy. This is
described in detail below. For a well commented example look at the linux userspace integration.
If you do not use CMake as your buildsystem, you may just build a static library once
and use the C++ API calls in libipstack/api.

Work in progress (August 2013):
* Make ipstack independent from CiAO operating system.
* Add linux userspace ipstack support via raw sockets or tun device.
* Add build system (after removing CiAO parts we need a complete replacement. Probably CMake)

Features:
=========
* BSD-like sockets API
* Dualstack IPv4/IPv6
* UDP
* TCP (Sliding Window, Avoid Silly Window Syndrome, Round-Trip Time Estimation, Congestion Control, MSS, ...)
* ICMPv4 info/error messages
* ICMPv6 info/error messages
* Neigbor discovery protocol (IPv6 - Address resolution, Duplication Check, Router discovery, Autoconfiguration)
* MLD (Multicast Listener Discovery) v1/v2
* ARP (IPv4 - Address resolution protocol)
* Only thin interface to your OS/APP has to be provided, IRQ safe

Configure for your needs:
=========================
* KConfig
TODO

Integration:
============
If your buildsystem is CMake based, you may want to use aspects to integrate this IP-Stack into your system.
This will be described in the next section. IF you just want a static library skip this section and read the next one.

Aspect-oriented integration:
----------------------------
TODO

Static-library integration:
---------------------------
You need CMake to build this software as a static lib. Create a directory "build" or any other name and
start cmake from there like this:
	cmake ../
If you use the GUI: Select the top directory as source and the "build" directory as build directory.
The configuration editor will be started, select the option __"Use as external static library"__.

Examples:
=========
TODO

Further reading:
================
* papers
