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

namespace ipstack {
	// These functions have to be delegated to network device abstraction
	class Interface {
	public:
		// All Interfaces are linked together for iterating
		Interface* getNext() { return next_interface; }
		
		/////// Name and Type of the interface ///////
		const char* getName() { return 0; }
		unsigned char getType() { return UNDEFINED; }
		
		/////// Maximum Transmission Unit ///////
		unsigned getMTU() { return 0; }

		/////// Link layer address ///////
		const unsigned char* getAddress() { return 0; }
		uint8_t getAddressSize() { return 0; }

		/////// Capabilities of your hardware ///////
		bool hasTransmitterHardwareChecksumming() { return false; }
		bool hasReceiverHardwareChecksumming() { return false; }
		
		/////// Sending frames ///////
		void send(const void* frame, unsigned framesize) {}
		/**
		 * For checksum offloading and networc card send offloading. This method is called
		 * if hasTransmitterHardwareChecksumming returns true.
		 * You get the data and datasize and additionally if it is tcp or udp and the size
		 * of the ip header.
		 */
		void send(const void* frame, unsigned framesize, bool tcp, unsigned ip_hdl) {}
		
		/**
		 * Return true if the frame has been transmitted by the hardware (and as a result
		 * if the sendbuffer can be freed. If you do not use DMA and the send method returns
		 * after sending the frame, you may always return true.
		 */
		bool hasBeenSent(const void* frame){ return true; }
		
		enum Interface_Type_Enum {
			UNDEFINED,
			ETHERNET,
			LOOPBACK,
			SLIP,
			RAWIPv4,
			RAWIPv6
		};
	private:
		Interface* next_interface; //linked list
	};

} // namespace ipstack
