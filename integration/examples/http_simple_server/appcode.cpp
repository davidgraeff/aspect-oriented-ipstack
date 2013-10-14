#include "stdio.h"
#include "tcp/TCP_Socket.h"
#include "tcp/TCP_Server.h"
#include "memory_management/Mempool_Instance_TCP.h"
#include "my_itoa.h"
#include "http_headers.h"
#include "../common/setupFirstInterface.h"

using namespace IPStack;

#define OUTPUTBUFFER_SIZE 1024
char buffer[OUTPUTBUFFER_SIZE];
char url[255];

void showPage(TCP_Socket* socket, char* filename, unsigned filenameLen) {
	printf("Request file: %s (%u)\n", filename, filenameLen);
	
	// OUTPUT BUFFER
	unsigned remainingLength = OUTPUTBUFFER_SIZE;
	char* pout = buffer;

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
		char errordata[] = "HTTP/1.1 500 Internal server error\r\n" 
			"Server: SimpleServe/1.0.0 (libipstack)\r\n\r\n"
			"<html><head><title>Server error</title></head><body>Server error</body></html>";
		socket->write(errordata,sizeof(errordata));
		return;
	}

	if (html) {
		// html
		memcpy(pout, html, html_len);
	}
	
	printf("Send %u bytes. Is 404? %u\n", pout-buffer+html_len, html_len==0);
	socket->write(buffer, pout-buffer+html_len);
}

void example_main() {
	setupFirstInterface<true, true>(); // enable ipv4/v6

	Mempool_Instance_TCP memory;
	TCP_Server<10> server(&memory, 80);
	TCP_Socket* clientSocket;
  
	while(1){
		printf("Wait for next request...");
		while (!(clientSocket=server->accept())) {};
		printf("client port %u\n", clientSocket->get_dport());
		
		int i=1;
	
		memset(buffer, 0, OUTPUTBUFFER_SIZE);
		i = clientSocket->receive(buffer, OUTPUTBUFFER_SIZE);
		char* input = buffer;
		unsigned url_size = parseURL(buffer, url, sizeof(url));
		
		if (url_size) {
			showPage(clientSocket, url, url_size);
		} else {
			printf("Empfangenen Inhalt nicht verstanden! %s\n", buffer); 
		}
		clientSocket->close();
	}
}

