// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2013 David Gräff

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/socket.h>

#ifdef linux
	#include <sys/ioctl.h>
	#include <linux/if.h>
	#include <linux/if_tun.h>
#endif

Tapdev* createByExistingTap(const char* tapdev = "/dev/tap0") {
	Tapdev* tap = new Tapdev();
	
	tap->fd = open(tapdev, O_RDWR);
	if(tap->fd == -1) {
		perror("Tapdev: createByExistingTap: open");
		delete tap;
		return 0;
	}
	
	tap->tapdeviceName = std::string(tapdev);
	unsigned found = tap->tapdeviceName.rfind("/");
	if (found!=std::string::npos)
		tap->tapdeviceName = tap->tapdeviceName.substr(found+1);
  
	sprintf(tap->ethernet_mac, " %02x:%02x:%02x:%02x:%02x:%02x", 
        (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	
	return tap;
}

Tapdev* createTap(const char* tapdevname = "tap%d") {
	Tapdev* tap = new Tapdev();
	
	tap->fd = open("/dev/net/tun", O_RDWR);
	if(tap->fd == -1) {
		perror("Tapdev: createTap: open");
		delete tap;
		return 0;
	}
  
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
	if( *dev )
         strncpy(ifr.ifr_name, tapdevname, IFNAMSIZ);
	
	if (ioctl(tap->fd, TUNSETIFF, (void *) &ifr) < 0) {
		perror();
		delete tap;
		return 0;
	}
	
	tap->tapdeviceName = std::string(ifr.ifr_name);
	
	sprintf(tap->ethernet_mac, " %02x:%02x:%02x:%02x:%02x:%02x", 
        (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	
	return tap;
}

Tapdev::Tapdev() {
	socketpair(PF_LOCAL, SOCK_DGRAM, 0, abort_fd);
}

bool Tapdev::setupIv4addr(const char* ipv4addr) {
  char buf[1024] = {0};
  snprintf(buf, sizeof(buf), "ifconfig %s inet %s/24", tap->tapdeviceName.c_str(), ipv4addr);
  system(buf);
}

void Tapdev::abortRead() {
	write(abort_fd[0],"1",1);
}

int Tapdev::read(char*& data, bool block) {
	fd_set fdset;
	struct timeval tv;
	timeval* tvp = 0;
	
	if (!block) {
		tv.tv_sec = 0;
		tv.tv_usec = 1000;
		tvp = &tv;
	}

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	FD_SET(abort_fd[0], &fdset);
	FD_SET(abort_fd[1], &fdset);

	int ret = select(abort_fd[1] + 1, &fdset, NULL, NULL, tvp);
	if(ret == 0) {
		return 0;
	}
	ret = read(fd, dataBuffer, dataBufferSize);
	if(ret == -1) {
		perror("tap_dev: tapdev_read: read");
	}
	data = dataBuffer;
	return ret;
}

bool Tapdev::write(const char* data, unsigned len) {
	int ret = write(fd, data, len);
	if(ret == -1) {
		perror("tap_dev: tapdev_send: writev");
		return false;
	}
	return true;
}
