#include "stdio.h"
#include "api/TCP_Socket.h"
#include "../common/setupFirstInterface.h"

using namespace IPStack;

void example_main() {
	setupFirstInterface<true, false>(); // enable ipv4 only

	ipstack::api::TCP_Socket<1514,1,128,2> socket;
	socket.set_sport(23); //server (telnet port)

	while(1){
		printf("Telnet listen: Connect with \"telnet ip %u\"\n", socket.get_sport());
		socket.listen();
		
		int i=1;
		char buffer[512];
		enum TelnetStateEnum {
			Unknown,
			PrintableCharacter,
			StartCommand,
			CommandDo,
			CommandWill
			
		} state;
		while(i>0){
		i = socket.receive(buffer, 512);
		state = Unknown;
		for(int j=0; j<i; ++j){
			const unsigned char c = (unsigned char)buffer[j];
			if (c == 0xff) {
				printf("Command ");
				state = StartCommand;
			} else if (state == StartCommand && c == 0xfd) {
				printf("Do> ");
				state = CommandDo;
			} else if (state == StartCommand && c == 0xfb) {
				printf("Will> ");
				state = CommandWill;
			} else if (state == CommandDo) {
				switch (c) {
					case 0x03: printf("Suppress go ahead\n"); break;
					case 0x05: printf("Status\n"); break;
					default:
						printf("Unknown\n"); break;
				}
				state = Unknown; // Reset state
			} else if (state == CommandWill) {
				switch (c) {
					case 0x18: printf("Terminate type\n"); break;
					case 0x1f: printf("Negotiate about window size\n"); break;
					case 0x20: printf("Terminate speed\n"); break;
					case 0x21: printf("Remote flow control\n"); break;
					case 0x22: printf("Linemode\n"); break;
					case 0x23: printf("X Display location\n"); break;
					case 0x27: printf("New environment option\n"); break;
					default:
						printf("Unknown\n"); break;
				}
				state = Unknown; // Reset state
			} else {
				if (state==Unknown) {
					printf("Telnet> ");
				}
				state = PrintableCharacter; // print state
				if (c != '\r')
					printf("%c", buffer[j]);
				if (c == 'Q' && i<=3) {
					// Quit with 'Q'+ENTER
					socket.close();
				}
			}
		}
		}
		while(socket.close() == false);
		printf("Terminated\n");
	}
}

