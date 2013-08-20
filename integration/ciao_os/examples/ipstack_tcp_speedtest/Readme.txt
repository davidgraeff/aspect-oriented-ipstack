This application is for testing the TCP Speed. It is in one of three states:
* listening for a command 
* receiving a predetermined amount of data as fast as possible
* sending a predetermined amount of data as fast as possible

It opens a tcp listen socket on port 1234 and starts in the listen-for-a-command state.

Commands:
* "LISTEN": The application sends back "OK"  and is ready to receive 1024*1024*100 Bytes (100 MByte)
	of data without further interpretation.
* "SEND": The application begins immediatelly to send 1024*1024*100 Bytes (100 MByte) of random data.

In the folder "linux_host_program" is a linux program located to use this application and measure the tcp performance. Usage:
speedtest 10.0.3.2 1234 LISTEN   or
speedtest 10.0.3.2 1234 SEND
