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
* Features are highly configurable
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
* KConfig
TODO

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

About multitasking support
--------------------------
The ipstack is designed for a multitasking system and an own task for the application execution.
For example we do some busy-waiting while waiting for an arp or IPv6 neighbour response and for timing
tcp traffic etc. Check out the CiAO-OS integration as an example: We reschedule before entering a busy
loop to allow for a better usage of the remaining waiting time. We use the alarm/event support of the
operating system to make the task sleep while waiting for the next packet or a tcp timeout.

But we realize that on some very restricted systems multitasking support costs valuable space or uses limited ressources like timers.
We therefore provide the __"BUILD_ONLY_ONE_TASK"__ cmake option. What it does is:
* Adding an ipstacl_periodic() method that has to be called periodically from your main loop.
* You need to check reachability before sending with IP::is_reachable(addr) because we no longer blocking the "application-task" while resolving link layer (e.g. ethernet) addresses.

About 8-bit µC
--------------
The software is not optimized for running on 8bit systems because we are using 16bit-minimum integers.
A comparison with other ipstacks on 8bit systems is a TODO.

Aspect-oriented integration
---------------------------
TODO
IRQ-safe

Static-library integration
--------------------------
In CMake:
* Select the option __"BUILD_ONLY_LIB"__.
* If you do not have a multitask system, you also need to check __"BUILD_ONLY_ONE_TASK"__.

The configuration editor will be started after clicking on generate.

Code-wise you have to setup the following:
* Send traffic received from your network card driver to _IP::receive_from_network(char* data, int len)_.
* Set the function pointer of _IP::send_to_network(char* data, int len)_ to an own function. This is called by the ipstack for outgoing traffic.
* call IP::init_ipstack()
* Provide functions for the function pointers IP::disable_irq() and IP::enable_irq() for IRQ/Interrupt safeness.

Examples
========
TODO

Further reading
===============
* papers
TODO