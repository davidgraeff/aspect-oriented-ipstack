Aspect Integration API
======================
[back](../readme.md)
TODO

You do not have to select any option in cmake as the Aspect-oriented integration way is the default way to build.
Code-wise you have to provide aspects to cover this functionally:
* Integrate _IP::init()_ into your initialize routines.
* Route received network traffic to IPStack::Router() __TODO__.
* Outgoing traffic can be accessed by an aspect with a pointcut to IPStack::SendBuffer::Send(char* data, int len) __TODO__.

Example: Look at `integration/linux_userspace_with_aspects`.
