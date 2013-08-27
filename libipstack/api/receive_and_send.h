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

/**
 * Only use the provided API of this file if you've build libipstack with the option
 * BUILD_WITH_ASPECTLESS_INTERFACE.
 * This file contains API to feed network traffic from your hardware driver into the
 * IPstack and to send data generated from the IPstack to your hardware.
 */

#pragma once
#include "../src/router/Router.h"

namespace IP {
	/**
	 * For each network device you have to add an interface. The interface object
	 * has to survive until removed with removeInterface! Initially after creating
	 * an interface object you have to call addInterface to make it known to the
	 * IPstack.
	 */
	
	// Register your interface to the stack.
	void addInterface(Interface*);
	// Unregister an interface
	void removeInterface(Interface*);
	
	/**
	 * Put received network traffic from your network device into this method.
	 * In a multitask system you also have to wake up your application task after
	 * calling this method to allow TCP/UDP sockets to consume the new network
	 * data. In a non-multitask system IP::periodic() will process the data.
	 */
	void receive_from_hardware(Interface* interface, char* data, int len);
	
	/**
	 * You have to implement a method with the signature below (be aware of the namespace!).
	 * This method is executed within the application task in a multitask system
	 * and provides new data to be send to the network device. The @interface
	 * identifies the device and @data and @len provide the data.
	 * 
	 * If your network device immediatelly copy the data into its own buffer set
	 * @freeBuffer to true. This allows the ipstack to reuse the buffers after this call.
	 * Otherwise if you are using some kind of DMA for example,
	 * set @freeBuffer to false. The @data pointer will be valid until
	 * you call free_sendbuffer(char* data).
	 */
	void send_to_hardware(Interface* interface, char* data, int len, bool& freeBuffer);

	/**
	 * You only need to call this if you've set @freeBuffer to false in send_to_hardware.
	 * You do not have to call this, but ipstacks memory management will then some time in the
	 * future start to ask you if buffers are usuable again (has_already_send()).
	 * This method will free the associated buffer and as a result the @data pointer is
	 * no longer valid. Do this if DMA operations finished.
	 */
	void free_sendbuffer(char* data);
	
	/**
	 * You have to implement a method with the signature below (be aware of the namespace!).
	 * This method will be called by the ipstacks memory management to free buffers if they
	 * have been send already. if you always set @freeBuffer to true, you won't get called by
	 * this method.
	 */
	bool has_already_send(char* data);
} // namespace IP
