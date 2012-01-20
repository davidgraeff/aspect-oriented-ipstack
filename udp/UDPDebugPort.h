#ifndef __IPSTACK_UDP_UDPDEBUGPORT_H__
#define __IPSTACK_UDP_UDPDEBUGPORT_H__

#include "../cfAttribs.h"
#include "ipstack/api/IPv4_UDP_Socket.h"

namespace ipstack {

class UDPDebugPort {
	public:
		static void putc_(char c) {
			static char OutBuffer[80];
			static unsigned fill = 0;
			OutBuffer[fill++] = c;
			
			if(fill>= sizeof(OutBuffer) // buffer full
					|| c == '\r' || c == '\n') { // flush character

				IP::IPv4_UDP_Socket<> socket;
				socket.set_dst_ipv4_addr(cfIPSTACK_UDP_DEBUGPORT_DESTIP);
				socket.set_dport(cfIPSTACK_UDP_DEBUGPORT_DESTPORT);
				socket.send(OutBuffer, fill);

				fill = 0;
			}
		}
};

}
#endif

