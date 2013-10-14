#include "stdio.h"
#include "string.h"
#include "api/TCP_Socket.h"
#include "../common/setupFirstInterface.h"

using namespace IPStack;

void example_main() {
	setupFirstInterface<true, true>(); // enable ipv4/v6

	TCP_Socket<1514,20,128,20> socket;
	socket.set_sport(1234);

	while(1) {
		printf("TCP Speedtest: Connect with \"speedtest %s %u LISTEN|SEND\"\n", "10.0.3.2", socket.get_sport());
		socket.listen();
		
		// Prepare buffers
		int byteCounter = 0;
		const int maxBytes = 1024*1024*100; //100MB
		char buffer[1514];
		enum StateEnum {
			CommandListenState,
			ReceiveState,
			SendingState
		} state = CommandListenState;
		int i=1;
		
		// Receive or send a lot of data
		while(i>0){
			i = socket.receive(buffer, sizeof(buffer));
			// Listen mode
			if (state==CommandListenState) {
				if (strncmp(buffer,"LISTEN", i)==0) {
					byteCounter = 0;
					printf("Change to receive mode\n");
					state = ReceiveState;
					socket.write("OK", 2);
				}
				else if (strncmp(buffer,"SEND", i)==0) {
					byteCounter = 0;
					printf("Change to send mode\n");
					state = SendingState;
				}
			}
			// Receiving mode
			if (state==ReceiveState) {
				printf(".");
				byteCounter += i;
				if (byteCounter>=maxBytes) {
					state = CommandListenState;
					printf("Change to listen mode\n");
				}
			}
			// Sending mode
			if (state==SendingState) {
				printf("Writing 100MB...\n");
				socket.write(buffer, maxBytes);
				state = CommandListenState;
				printf("Change to listen mode\n");
			}
		}
		
		// Close socket and wait until state changed to close
		while(socket.close() == false);
		printf("Finished\n");
	}
}
