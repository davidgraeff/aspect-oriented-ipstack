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
// Copyright (C) 2011 Christoph Borchert, 2013-2013 David Graeff

#pragma once
#include "util/ipstack_inttypes.h"
#include "router/Interface.h"
#include "tapdev.h"

// These functions have to be delegated to network device abstraction
class TapInterface {
public:
	TapInterface(Tapdev* tapdev) : tapdev(tapdev) {}
	
	const char* getName() { return "tap device"; }
	unsigned char getType() { return ipstack::Interface::ETHERNET; }
	
	/////// Maximum Transmission Unit ///////
	unsigned getMTU() { return tapdev->getMTU(); }

	/////// Link layer address ///////
	const unsigned char* getAddress() { return tapdev->getEthernetAddress(); }
	uint8_t getAddressSize() { return tapdev->getEthernetAddressSize(); }

	/////// Capabilities of your hardware ///////
	bool hasTransmitterHardwareChecksumming() { return false; }
	bool hasReceiverHardwareChecksumming() { return false; }
	
	/////// Sending frames ///////
	void send(const void* frame, unsigned framesize) {
		tapdev->write(frame, framesize);
	}
	
	void send(const void*, unsigned, bool, unsigned) {}
	bool hasBeenSent(const void*){ return true; }
private:
	Tapdev* tapdev;
};
