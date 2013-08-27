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
// Copyright (C) 2011 Christoph Borchert

/**
 * Methods for getting access to the interfaces. Usually you would use it like this:
 * ipstack::Interface* interface = IP::getInterface(0);
 * if (interface) {
 *    const UInt8* hw_addr = interface->getAddress(); // ethernet mac
 *    interface->setIPv4Addr(10,0,3,2); // set ipv4 addr
 *    interface->setIPv4Subnetmask(255,255,255,0); // set ipv4 subnet mask
 *    interface->setIPv4Up(true); // activate
 * }
 */

#pragma once
#include "../src/router/Router.h"

namespace IP {
	/**
	 * Return interface numbered with @num. If no
	 * interface with this number exists 0 is returned.
	 */
	ipstack::Interface* getInterface(unsigned num){
		return Router::Inst().get_interface(num);
	}
} // namespace IP
