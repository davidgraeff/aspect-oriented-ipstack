#include "appcode.h"
#include "as/AS.h"
#include "ipstack/as/EventSupport.h"
#include "objects.h"
#include "stdio.h"
#include "string.h"

#include "ipstack/api/Setup.h"
#include "ipstack/api/TCP_Socket.h"

/*** ---------------------------------------------------------*/
ipstack::Interface* setupEthernet() {
  printf("Setting up ipv4/ipv6 networking ...\n\n");
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

    interface->setIPv6Up(true);
    printf("IPv6-Status: Up (Running)\n\n");
  }
  return interface;
} // *** ---------------------------------------------------------


IP::TCP_Socket<1514,50,128,50> socket;

void Alpha::functionTaskTask0() {
  ipstack::Interface* interface = setupEthernet();
  if (interface==0) {
		// ethernet setup failed
		printf("No ethernet card found!\n");
		while(1);
  }

  new(&socket) IP::TCP_Socket<1514,50,128,50>;
  socket.setAlarm(TCPAlarmTask0);
  socket.set_sport(1234);

  while(1){
	printf("TCP Speedtest: Connect with \"speedtest %s %u LISTEN|SEND\"\n", "10.0.3.2", socket.get_sport());
    socket.listen();
    // Prepare buffers
    int byteCounter = 0;
	const int maxBytes = 1024*1024*100;
    char buffer[1514];
	enum StateEnum {
		CommandListenState,
		ReceiveState,
		SendingState
	} state = CommandListenState;
	int i=1;
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
    while(socket.close() == false);
    printf("Terminated\n");
  }
}

