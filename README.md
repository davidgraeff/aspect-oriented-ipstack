Aspect oriented IPstack for embedded systems
============================================
A highly configurable and easily extendable TCP/IP-Stack with IPv4, IPv6, UDP, TCP, ICMPv4, ICMPv6 support.
This network stack is written in AspectC++ and because it is build around the idea of aspects you
may select your desired functionallity during build time in a very fine grained way.

The aspect-IP-Stack is plattform agnostic and supports little and big endian systems and have been tested
on x86 32/64bit as well as arm 32bit and msp430 16bit processors. We are going to test the ipstack on 8bit
processors especially avr µC's in the near future.

You may use this software in different ways.
There are two examples of integrating the IP-Stack into operating systems for embedded systems (CiAO-OS and Chibi-OS)
and another example of using it as a linux userspace application on a tun-device.

If you already use the CMake buildsystem, integrating the IP-Stack is very easy. This is
described in detail below. For a well commented example look at the linux userspace integration.
If you do not use CMake as your buildsystem, you may just build a static library once
and use the C++ API calls in libipstack/api.

Work in progress (August 2013):
-------------------------------
This software is currently NOT USABLE as it is.
* Work on this documentation
* Build system, CMake
* Make ipstack independent from CiAO operating system.
* Add linux userspace ipstack support via raw sockets or tun device.
* Add API for non-multitasking support
* Add build system parts for adding own aspects outside of the libipstack/src directory.
* Add documentation papers to /doc.

Features
========
* Highly configurable at build time
* BSD-like sockets API
* Own memory management for ressource restricted systems. We not not use the heap in the default configuration.
* Only thin interface to your OS/APP has to be provided, IRQ safe

Link-Layer:
* Ethernet
* ARP (IPv4 - Address resolution protocol)

IP-Layer:
* Dualstack IPv4/IPv6
* ICMPv4/v6 info/error messages (ping, etc)
* Neigbor discovery protocol (IPv6 - Address resolution, Duplication Check, Router discovery, Autoconfiguration)
* MLD (Multicast Listener Discovery) v1/v2

Transport-Layer:
* UDP
* TCP (Sliding Window, Avoid Silly Window Syndrome, Round-Trip Time Estimation, Congestion Control, MSS, ...)

Configure for your needs
========================
The configuration of this software is done by the kconfig tool, developted and used by the linux-kernel community.
We're using a slightly modified version, the __kconfig-frontends__ packet (http://ymorin.is-a-geek.org/projects/kconfig-frontends).

__TODO__: Picture of kconfig

Building and execution of kconfig is integrated into the cmake buildsystem. It is even prepared for the case that
you already using kconfig for your os or application.

Integration
===========
If your buildsystem is CMake based, you may want to use aspects to integrate this IP-Stack into your system.
This allows you more complex integration like using the a networks cards capability of calculating checksums etc
and is described in the _Aspect-oriented integration_ section. In contrast if you just want to provide an interface for connecting the
ipstack with your network hardware and use it as it is skip this section and read the _Static-library integration_ one.

General procedure for cmake
---------------------------
You need CMake to build this software in either way. You may obtain it at http://www.cmake.org.
We only explain the graphical way of configuring the build system (cmake-gui), but you may also use
the command line version.
* Create a directory called "build" (or any other name).
* Start cmake-gui and select the top directory as source and the "build" directory as build directory.
* Click __"configure"__.
You will be presented with some build options that are discussed in the following sections.
The configuration editor will be started after clicking on configure in cmake for the first time. If you want to reconfigure,
select __KCONFIG_RECONFIGURE__.

About multitasking support
--------------------------
The ipstack is designed for a multitasking system and an own task for the application execution.
For example we do some busy-waiting while waiting for an arp or IPv6 neighbour response and for timing
tcp traffic etc. Check out the CiAO-OS integration as an example: We reschedule before entering a busy
loop to allow for a better usage of the remaining waiting time. We use the alarm/event support of the
operating system to make the task sleep while waiting for the next packet or a tcp timeout.

But we realize that on some very restricted systems multitasking support costs valuable space or uses limited ressources like timers.
We therefore provide the __"BUILD_ONLY_ONE_TASK"__ cmake option. What it does is:
* Adding an _IP::periodic()_ method that has to be called periodically from your main loop.
* You need to check reachability before sending with _IP::is_reachable(addr)_ because we no longer blocking the "application-task" while resolving link layer (e.g. ethernet) addresses.

Example: Look at integration/linux_userspace_without_aspects_multitask

About 8-bit µC
--------------
The software is not optimized for running on 8bit systems because we are using 16bit-minimum integers.
A comparison with other ipstacks on 8bit systems is a __TODO__.

IRQ-Safeness, timing and fatal-errors
-------------------------------------
Provide functions for the function pointers IP::disable_irq() and IP::enable_irq() for IRQ/Interrupt safeness.

Aspect-oriented integration
---------------------------
Code-wise you have to provide aspects to cover this functionally:
* Integrate _IP::init()_ into your initialize routines.
* Route received network traffic to IPStack::Router() __TODO__.
* Outgoing traffic can be accessed by an aspect with a pointcut to __TODO__.

Example: Look at integration/linux_userspace_with_aspects

Static-library integration
--------------------------
In CMake:
* Select the option __"BUILD_ONLY_LIB"__.
* If you do not have a multitask system, you also need to check __"BUILD_ONLY_ONE_TASK"__.

Code-wise you have to setup the following:
* call _IP::init()_ before using any of the ipstack methods.
* Route traffic received from your network card driver to _IP::receive_from_network(char* data, int len)_.
* Implement a function for sending to your network card driver and set the function pointer of _IP::send_to_network(char* data, int len)_ accordingly. This is called by the ipstack for outgoing traffic.

Example: Look at integration/linux_userspace_without_aspects

Examples
========
We provide some example applications and example integrations. The applications aren't executable on their own without an integration.

Example applications
--------------------
http_simple_server: __TODO__

icmp_test: __TODO__

tcp_speedtest: __TODO__

telnet: __TODO__

Example integrations
--------------------
chibi_os: __TODO__

ciao_os: __TODO__

linux_userspace_with_aspects: __TODO__

linux_userspace_without_aspects: __TODO__

linux_userspace_without_aspects_multitask: __TODO__

Further reading
===============
* papers
__TODO__