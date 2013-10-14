This application is for testing ICMPv4/v6, udp send/receive, tcp reset
You can ping libipstack by using
* ping 10.0.3.2 (for icmp ping)
* ping6 fe80::6655:44ff:fe33:2211%tap0 (for icmp ping with ipv6 on tap0 device)
* echoping -u 10.0.3.2 (for udp ping; use ipv4 addresses, ipv6 is not supported by echoping for udp)
* sendip -p ipv6 -p udp -us 5070 -ud 7 -d "Hello" -v fe80::6655:44ff:fe33:2211 (for ipv6 udp ping)
* telnet 10.0.3.2 (for testing the tcp reset capability; use ipv4 or ipv6 address)
* Send udp to port 88 to get it printed out

For full network interaction
* activate ipv6 forwarding: sysctl -w net.ipv6.conf.all.forwarding=1
* have an ipv6 capable router (or the "radvd" software)
