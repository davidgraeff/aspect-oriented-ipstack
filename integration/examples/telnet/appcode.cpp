#include "stdio.h"

#include "api/Setup.h"
#include "api/TCP_Socket.h"

/*** ---------------------------------------------------------*/
ipstack::Interface* setupEthernet() {
  printf("Setting up ipv4 networking ...\n\n");
  ipstack::Interface* interface = IP::getInterface(0);
  if(interface != 0){
    printf("Device #0: %s\n", interface->getName() );

    printf("MAC-Address: ");
    const UInt8* hw_addr = interface->getAddress();
    for(int i=0; i<5; ++i){
      printf("%X:", hw_addr[i]);
    }
    printf("%X\n", hw_addr[5]);
    
    printf("MTU: %u\n", interface->getMTU());

	// Prepare the interface for an IPv4 connection
    interface->setIPv4Addr(10,0,3,2);
    printf("IPv4-Address: 10.0.3.2\n");

    interface->setIPv4Subnetmask(255,255,255,0);
    printf("IPv4-Subnetmask: 255.255.255.0\n");

    interface->setIPv4Up(true);
    printf("IPv4-Status: Up (Running)\n\n");
  }
  return interface;
} // *** ---------------------------------------------------------


IP::TCP_Socket<1514,1,128,2> socket; //alloc on bss as global object (variant 1)

void example_main() {
  ipstack::Interface* interface = setupEthernet();
  if (interface==0) {
		// ethernet setup failed
		printf("No ethernet card found!\n");
		while(1);
  }

  //ipstack::api::TCP_Socket<1514,1,128,2> socket; //alloc on stack (variant 2)
  new(&socket) IP::TCP_Socket<1514,1,128,2>; //explicitly call constructor for global object (only for variant 1)
  //socket.setAlarm(TCPAlarmTask0);
  socket.set_sport(23); //server (telnet port)

  while(1){
	printf("Telnet listen: Connect with \"telnet %s %u\"\n", "10.0.3.2", socket.get_sport());
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

//   while(1) {
// 	  AS::WaitEvent(IPStackReceiveEvent_);
// 	  printf(".");
//   }
}

