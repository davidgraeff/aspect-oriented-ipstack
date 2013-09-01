Aspectless Integration API
==========================
[back](../readme.md)

In CMake:
* Select the option __"BUILD_ONLY_LIB"__.
* If you do not have a multitask system, you also need to check __"BUILD_ONLY_ONE_TASK"__.

Code-wise you have to setup the following:
* call _IP::init()_ before using any of the ipstack methods.
* Route traffic received from your network card driver to _IP::receive_from_network(char* data, int len)_.
* Implement a function for sending to your network card driver and set the function pointer of _IP::send_to_network(char* data, int len)_ accordingly. This is called by the ipstack for outgoing traffic.

Example: Look at `integration/linux_userspace_without_aspects`.
