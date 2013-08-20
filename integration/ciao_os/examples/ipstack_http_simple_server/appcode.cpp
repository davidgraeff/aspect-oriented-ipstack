#include "appcode.h"
#include "as/AS.h"
#include "ipstack/as/EventSupport.h"
#include "objects.h"
#include "stdio.h"
#include "ipstack/api/Setup.h"
#include "ipstack/api/TCP_Socket.h"

/*** ---------------------------------------------------------*/
ipstack::Interface* setupEthernet() {
	ipstack::Interface* interface = IP::getInterface(0);
	if (!interface) {
		printf("Setting up networking... failed\n");
	} else {
		printf("Setting up network for device #0: %s\n", interface->getName() );

		printf("  MAC-Address: ");
		const UInt8* hw_addr = interface->getAddress();
		for(int i=0; i<5; ++i){
		printf("%X:", hw_addr[i]);
		}
		printf("%X\n", hw_addr[5]);
		
		printf("  MTU: %u\n", interface->getMTU());

		// Prepare the interface for an IPv4 connection
		// Uncomment the following lines for IPv4 functionallity
		interface->setIPv4Addr(10,0,3,2);
		printf("  IPv4-Address: 10.0.3.2\n");

		interface->setIPv4Subnetmask(255,255,255,0);
		printf("  IPv4-Subnetmask: 255.255.255.0\n");

		interface->setIPv4Up(true);
		printf("  IPv4-Status: Up (Running)\n");

		interface->setIPv6Up(true);
		printf("  IPv6-Status: Up (Running)\n");
		UInt8 nextEntry = 0;
		while (AddressEntry* entry = interface->addressmemory.findEntry<AddressEntryType>(&nextEntry)) {
			char buffer[50] = {0};
			ipstack::ipv6_addr_toString(entry->ipv6, buffer);
			printf("  IPv6-Address: %s\n", buffer);
		}
	}
	return interface;
} // *** ---------------------------------------------------------

/**
  * Convert integer to string
  */
int my_itoa(int val, char* buf)
{
    char* p = buf;

	// negativ?: add - sign
    if (val < 0) {
        *p++ = '-';
        val = 0 - val;
    }

	// convert digit per digit to ascii chars
    char* b = p;
    unsigned int digit;
    unsigned int restVal = (unsigned int)val;
    do {
        digit = restVal % 10;
        restVal /= 10;
        *p++ = digit + '0'; // make ascii char out of digit
    } while (restVal > 0);

    int len = (int)(p - buf);

	// last character is 0 (decrease buffer pointer after that)
    *p-- = 0;

    // swap
    char temp;
    do {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}

void parseError(char* buffer) {
	printf("Empfangenen Inhalt nicht verstanden! %s\n", buffer); 
}

bool addErrorResponseHeader(char*& destbuffer, unsigned& destRemainingLen) {
	char header1[] = "HTTP/1.1 404 NOT FOUND\r\n" 
	"Server: SimpleServe/1.0.0 (CiAO)\r\n"
	"Content-Length: 0\r\n";
	if (destRemainingLen<sizeof(header1)-1)
		return false;
	
	memcpy(destbuffer, header1, sizeof(header1)-1);
	destbuffer+=sizeof(header1)-1;
	destRemainingLen-=(sizeof(header1)-1);
	
	return true;
}

bool addResponseHeader(char*& destbuffer, unsigned& destRemainingLen, unsigned contentLen) {
	char header1[] = "HTTP/1.1 200 OK\r\n" 
	"Server: SimpleServe/1.0.0 (CiAO)\r\n"
	"Content-Length: ";
	char sizeOfHtmlString[20];
	unsigned char len = my_itoa(contentLen, sizeOfHtmlString);
		
	if (destRemainingLen<sizeof(header1)-1+len+2)
		return false;
	
	// copy header part+ string of length + "\r\n"
	memcpy(destbuffer, header1, sizeof(header1)-1);
	destbuffer+=sizeof(header1)-1;
	memcpy(destbuffer, sizeOfHtmlString, len);
	destbuffer+=len;
	destbuffer[0] = '\r';
	destbuffer[1] = '\n';
	destbuffer+=2;
	destRemainingLen-=(sizeof(header1)-1)-len-2;
	
	return true;
}

bool addContentLanguageHeader(char*& destbuffer, unsigned& destRemainingLen) {
	char h[] = "Content-Language: de\r\n";
	if (destRemainingLen<sizeof(h)-1)
		return false;
	
	memcpy(destbuffer, h, sizeof(h)-1);
	destbuffer+=sizeof(h)-1;
	destRemainingLen -= (sizeof(h)-1);
	return true;
}

bool addConnectionClose(char*& destbuffer, unsigned& destRemainingLen) {
	char h[] = "Connection: close\r\n";
	if (destRemainingLen<sizeof(h)-1)
		return false;
	
	memcpy(destbuffer, h, sizeof(h)-1);
	destbuffer+=sizeof(h)-1;
	destRemainingLen -= (sizeof(h)-1);
	return true;
}

bool addContentType(char*& destbuffer, unsigned& destRemainingLen, char* contentType) {
	char h[] = "Content-Type: ";
	unsigned len = strlen(contentType);
	if (destRemainingLen<sizeof(h)-1+len+2)
		return false;
	
	memcpy(destbuffer, h, sizeof(h)-1);
	destbuffer+=sizeof(h)-1;
	memcpy(destbuffer, contentType, len);
	destbuffer+=len;
	destbuffer[0] = '\r';
	destbuffer[1] = '\n';
	destbuffer+=2;
	destRemainingLen -= (sizeof(h)-1)-len-2;
	return true;
}

bool closeHeader(char*& destbuffer, unsigned& destRemainingLen) {
	if (destRemainingLen<2)
		return false;
	
	destbuffer[0] = '\r';
	destbuffer[1] = '\n';
	destbuffer+=2;
	destRemainingLen-=2;
	
	return true;
}

IP::TCP_Socket<1514,1,128,2> socket; //alloc on bss as global object (variant 1)

void showPage(char* filename, unsigned filenameLen) {
	printf("Request file: %s (%u)\n", filename, filenameLen);

	// CONTENT
	char* html = 0;
	unsigned html_len;
	char* contentType;
	if (memcmp(filename,"/",filenameLen)==0) { // index page
		char data[] = "<html><head><title>Testseite</title></head><body><h1>Testseite</h1><p>Hier steht Text</p><a href='speedtest.bin'>Lade große Datenmenge für Speedtest</a></body></html>";
		html_len = sizeof(data)-1;
		html = data;
		contentType = "text/html; charset=utf-8;";
	} else if (memcmp(filename,"/speedtest.bin",filenameLen)==0) { // speedtest file
		html_len = 1024*1024*10; // 10 MB
		contentType = "application/octet-stream";
	} else { // not found
		html_len = 0;
	}

	// HEADER
	unsigned remainingLength = 1024;
	char output[remainingLength];
	char* pout = output;
	bool ok = true;
	
	if (!html_len)
		ok &= addErrorResponseHeader(pout, remainingLength);
	else
		ok &= addResponseHeader(pout, remainingLength, html_len);
	ok &= addContentLanguageHeader(pout, remainingLength);
	ok &= addConnectionClose(pout, remainingLength);
	if (html_len)
		ok &= addContentType(pout, remainingLength, contentType);
	ok &= closeHeader(pout, remainingLength);
	
	if (!ok) {
		while(socket.close() == false);
		return;
	}

	if (html) {
		// html
		memcpy(pout, html, html_len);
	}
	
	printf("Send %u bytes. Is 404? %u\n", pout-output+html_len, html_len==0);
	socket.write(output, pout-output+html_len);
	while(socket.close() == false);
}

void Alpha::functionTaskTask0() {
  ipstack::Interface* interface = setupEthernet();
  if (interface==0) {
		// ethernet setup failed
		printf("No ethernet card found!\n");
		while(1);
  }

  //ipstack::api::TCP_Socket<1514,1,128,2> socket; //alloc on stack (variant 2)
  new(&socket) IP::TCP_Socket<1514,1,128,2>; //explicitly call constructor for global object (only for variant 1)
  socket.setAlarm(TCPAlarmTask0);
  socket.set_sport(80); //server (telnet port)

	while(1){
		printf("Wait for request: Connect with \"http://%s:%u\"\n", "10.0.3.2", socket.get_sport());
		socket.listen();
		
		int i=1;
		char buffer[512];
		char url[100];
	
		memset(buffer, 0, 512);
		i = socket.receive(buffer, 512);
		char* input = buffer;
		
		/// Parse requested url (example: GET /infotext.html HTTP/1.1) ///
		if (memcmp(input, "GET", 3)==0) {
			input += 4;
			unsigned pos = 0;
			for (;pos<99;++pos) {
				char c = input[pos];
				if (c == ' ') break;
				url[pos] = c;
			}
			url[pos] = 0; // last character is 0 to terminate the string
			input += pos + 1; // jump to the character after the whitespace after the url
// 			printf("Content: %s\n", input);
			if (memcmp(input, "HTTP/1", 6)==0) {
				showPage(url, pos);
			} else {
				printf("Request failed: %s\n", url);
				parseError(buffer);
			}
		} else {
			parseError(buffer);
		}

		while(socket.close() == false);
		printf("Terminated\n");
	}
}

