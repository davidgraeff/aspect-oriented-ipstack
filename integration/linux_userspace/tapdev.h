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

#pragma once
#include <string>
#include <inttypes.h>

class Tapdev {
	public:
		// No root required
		static Tapdev* createByExistingTap(const char* tapdev = "/dev/tap0");
		// Root permissions required
		static Tapdev* createTap(const char* tapdev = "tap%d");
		// Root permissions required (ifconfig execute)
		bool setupIv4addr(const char* ipv4addr = "192.168.0.20");
		
		void abortRead();
		int read(char*& data, bool block = true);
		bool write(const char* data, unsigned len);
		
		inline int getMTU() const { return mtu; }
		inline unsigned char* getEthernetAddress() const { return ethernet_mac; }
		inline unsigned getEthernetAddressSize() const { return 6; }
	private:
		// handlers
		int fd;
		int abort_fd[2];
		// ethernet attributes
		int mtu;
		unsigned char ethernet_mac[19];
		std::string tapdeviceName;
		// receive buffer
		enum {dataBufferSize = 1500};
		char dataBuffer[dataBufferSize];
	private:
		Tapdev() {} // private constructor
};
