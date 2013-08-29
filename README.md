Aspect oriented IPstack for embedded systems
============================================
A highly configurable and easily extendable TCP/IP-Stack with IPv4, IPv6, UDP, TCP, ICMPv4, ICMPv6 support.
This network stack is written in AspectC++ and because it is build and designed around the idea of aspects you
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

[![Build Status](https://travis-ci.org/davidgraeff/aspect-oriented-ipstack.png?branch=master)](https://travis-ci.org/davidgraeff/aspect-oriented-ipstack)
Please use the ticket system of github for bugs and feature suggestions.

Work in progress (August 2013):
-------------------------------
This software is currently **NOT USABLE** in its current state. TODO:
* Work on this documentation
* Add linux userspace ipstack support via raw sockets or tun device.
* Add API for non-multitasking support
* Add build system parts for adding own aspects outside of the libipstack/src directory.
* Add documentation papers to /doc (e.g. APIs, example step-by-step).

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

![Picture of kconfig](doc/kconfig.png)

Building and execution of kconfig is integrated into the cmake buildsystem. It is even prepared for the case that
you already using kconfig for your os or application.

Build and Integrate
===================
If your buildsystem is CMake based, you may want to use aspects to integrate this IP-Stack into your system.
This allows more complex integration like using network cards capability of calculating checksums etc
and is described in the _Aspect-oriented integration_ section. In contrast if you just want to provide an interface for connecting the
ipstack with your network hardware and use it as it is skip that section and read the _Static-library integration_ one.

General procedure for building
------------------------------
You need CMake to build this software in either way. You may obtain it at http://www.cmake.org.
Additionally you need the aspectc++ compiler from http://www.aspectc.org.
We only explain the graphical way of configuring the build system (cmake-gui), but you may also use
the command line version.
* Create a directory called "build" (or any other name).
* Start cmake-gui and select the top directory as source and the "build" directory as build directory.
* Click __"configure"__.
* A popup will appear and ask for the target build system. We assume you are using `make`.
![Picture of cmake](doc/cmake.png)
You will be presented with some build options that are discussed in the following sections. If you are done
with configuring the buildsystem click on click __"generate"__. Depending on your selection in the first
popup you have VisualStudio project files, make files or something else in your build directory.
* Change to your build directory.
* Execute `make`. This will download and build the kconfig-frontend and automatically open the configuration editor.
* Configure the ipstack to your needs. Close the editor and save the changes.
* Execute `make` again to build the examples if selected and the libipstack library.

If you want to reconfigure libipstack do this:
* Change to your build directory.
* Execute `make reconfigure`. This will open the configuration editor.
* Configure the ipstack to your needs. Close the editor and save the changes.
* Execute `make` again to build the examples if selected and the libipstack library.

About multitasking support
--------------------------
The ipstack is designed for multitasking systems. Usually there exists the system task
where interaction with the hardware takes place and traffic is received. The second 
the user task where traffic is generated and consumed. Places where tasks are used:
For example we do some busy-waiting while waiting for an arp or IPv6 neighbour response and for timing
tcp traffic etc. Check out the CiAO-OS integration as an example: We reschedule before entering a busy
loop to allow for a better usage of the remaining waiting time. We use the alarm/event support of the
operating system to make the task sleep while waiting for the next packet or tcp timeout.

But we realize that on some very restricted systems multitasking support costs valuable space or uses limited ressources like timers.
We therefore provide the __"BUILD_ONLY_ONE_TASK"__ cmake option. What it does is:
* Adding an _IP::periodic()_ method that has to be called periodically from your main loop.
* You need to check reachability before sending with _IP::is_reachable(addr)_ because we no longer blocking the "application-task" while resolving link layer (e.g. ethernet) addresses.

Example: Look at `integration/linux_userspace_without_aspects_multitask`.

Aspect-oriented integration
---------------------------
You do not have to select any option in cmake as the Aspect-oriented integration way is the default way to build.
Code-wise you have to provide aspects to cover this functionally:
* Integrate _IP::init()_ into your initialize routines.
* Route received network traffic to IPStack::Router() __TODO__.
* Outgoing traffic can be accessed by an aspect with a pointcut to IPStack::SendBuffer::Send(char* data, int len) __TODO__.

Example: Look at `integration/linux_userspace_with_aspects`.

Static-library integration
--------------------------
In CMake:
* Select the option __"BUILD_ONLY_LIB"__.
* If you do not have a multitask system, you also need to check __"BUILD_ONLY_ONE_TASK"__.

Code-wise you have to setup the following:
* call _IP::init()_ before using any of the ipstack methods.
* Route traffic received from your network card driver to _IP::receive_from_network(char* data, int len)_.
* Implement a function for sending to your network card driver and set the function pointer of _IP::send_to_network(char* data, int len)_ accordingly. This is called by the ipstack for outgoing traffic.

Example: Look at `integration/linux_userspace_without_aspects`.

Examples
========
We provide some example applications and example integrations.
The applications aren't executable on their own without an integration.

Example applications
--------------------
> __http_simple_server:__ Provides a very simple http server. Only one page
> is returned and the http headers and content are statically compiled in.

> __icmp_test:__ Only the ip management subsystem is enabled.
> This application is for testing ICMPv4/v6, udp send/receive, tcp reset.
> You can ping by using:
> * ping 10.0.3.2 (for icmp ping, substitue with your configured IP)
> * ping6 fe80::6655:44ff:fe33:2211%tap0 (for icmp ping with ipv6 on tap0 device)
> * echoping -u 10.0.3.2 (for udp ping; use ipv4 addresses, ipv6 is not supported by echoping for udp)
> * sendip -p ipv6 -p udp -us 5070 -ud 7 -d "Hello" -v fe80::6655:44ff:fe33:2211 (for ipv6 udp ping)
> * telnet 10.0.3.2 (for testing the tcp reset capability; use ipv4 or ipv6 address)
> * Send udp to port 88 to get it printed out
> 
> For full network interaction
> * activate ipv6 forwarding: sysctl -w net.ipv6.conf.all.forwarding=1
> * have an ipv6 capable router (or the "radvd" software)

> __tcp_speedtest:__ This application is for testing the TCP Speed. It is in one of three states:
> * listening for a command 
> * receiving a predetermined amount of data as fast as possible
> * sending a predetermined amount of data as fast as possible
> 
> It opens a tcp listen socket on port 1234 and starts in the listen-for-a-command state. Commands:
> * "LISTEN": The application sends back "OK"  and is ready to receive 1024*1024*100 Bytes (100 MByte)
> 	of data without further interpretation.
> * "SEND": The application begins immediatelly to send 1024*1024*100 Bytes (100 MByte) of random data.
> 
> In the directory "linux_host_program" is a linux host program located for measuring tcp performance.
> Usage: _speedtest 10.0.3.2 1234 LISTEN_ or _speedtest 10.0.3.2 1234 SEND_.

> __telnet:__ Use the telnet application to test this example. An echo to every
> message is returned.

Example integrations
--------------------
__linux_userspace_with_aspects:__ In this example we're using aspects to tie
the ipstack and the linux tun device together. Two threads are initialized.
One is for receiving traffic and feeding the ipstack buffers and the other one
is for the application for generating and consuming traffic. All application
examples from above can be used with this type of integration.

__linux_userspace_without_aspects:__ The same as above, but the application
example, tun-device and threads are tied together without aspects. This example
demonstrates how to compile the ipstack as a standalone static lib that can
be used from outside by just using the api methods.

__linux_userspace_without_aspects_multitask:__ This example demonstrates the usage
of the ip-stack without using a multitask system. The main loop is responsible for
routing incoming and outgoing traffic to and from the ipstack and at the same time
provide the application layer. We show how to use the IP::periodic() and
IP::is_reachable(...) methods.

__ciao_os:__ The ipstack origins at this operating system for embedded systems. Therefore
support and maintainance are good for this integration. CiAO implements AutoSar
Events and Alarms and is multitasking capable. Usually there exists the system task
where traffic is received and the user task where traffic is generated and consumed.
The integration features IRQ-safeness, reschedules instead of inefficent waits and
task sleep if waiting for a packet.

**chibi_os**: __TODO__

Further reading
===============
* papers
__TODO__

License
=======
The buildsystem, example and integration code is licensed under the terms of the BSL 2-clause license.
The ipstack source code is GPL3 licensed. Please be aware of the implication: Because this software
is usally build as static library, your code have to be GPL3 compatible code.
