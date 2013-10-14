#pragma once 


bool addErrorResponseHeader(char*& destbuffer, unsigned& destRemainingLen) {
	char header1[] = "HTTP/1.1 404 NOT FOUND\r\n" 
	"Server: SimpleServe/1.0.0 (libipstack)\r\n"
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
	"Server: SimpleServe/1.0.0 (libipstack)\r\n"
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

/**
 * Parse requested url (example: GET /infotext.html HTTP/1.1)
 * @param url Contains the url for instance "/infotext.html"
 * @return Return size of url or 0 if invalid data
 */
unsigned parseURL(char* http_data, char* url, unsigned urlSize) {
	
	/// Parse requested url (example: GET /infotext.html HTTP/1.1) ///
	if (memcmp(http_data, "GET", 3)==0) {
		http_data += 4;
		unsigned pos = 0;
		for (;pos<urlSize;++pos) {
			char c = http_data[pos];
			if (c == ' ') break;
			url[pos] = c;
		}
		url[pos] = 0; // last character is 0 to terminate the string
		http_data += pos + 1; // jump to the character after the whitespace after the url
		if (memcmp(http_data, "HTTP/1", 6)==0) {
			return pos;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}