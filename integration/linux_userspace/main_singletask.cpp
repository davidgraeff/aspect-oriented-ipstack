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

#include "tapdev.h"
#include <iostream>
#include <memory>
#include "appcode.h"
#include "tapinterface.h"
#include "demux/Demux.h"
#include "router/Interface.h"

#include <stdio.h>

TapInterface* tap;
int main(int argc, char* argv[])
{

	std::auto_ptr<TapDev> tap(TapDev::createTap());
	if (!tap.get()) {
		std::cerr << "Error, no tap device created!\n";
		return 1;
	}
	tapPointer = tap.get();
	
	// init ipstack
	ipstack::System::init();
	
	// get router and demux object
	ipstack::Demux& demux = ipstack::Demux::Inst();
	ipstack::Router& router = ipstack::Router::Inst();

	// In this thread we just receive (and send) 
	tap = new TapInterface(tap.get());
	router.add_interface(tap);
	
	// run example_main().
	example_main();

	delete tap;
	return 0;
}

void mainloop() {
	char* data;
	// no blocking
	unsigned len = tap->read(data, false);
	demux.demux(data, len, tap);
	// do periodic tasks 
	ipstack::System::periodic();
}
