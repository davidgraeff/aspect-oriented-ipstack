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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool abort_execution = false;

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
TapDev* tapPointer;
void signal_callback_handler(int signum)
{
	std::cerr << "Caught signal " << signum << std::endl;

	// Terminate program
	tapPointer->abortRead();
	abort_execution = true;
}

void* execute_example(void *threadid) {
	example_main();
}

int main(int argc, char* argv[])
{
	// Register signal and signal handler
	signal(SIGINT, signal_callback_handler);

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
	std::auto_ptr<TapInterface> tapInterface(new TapInterface(tap.get()));
	router.add_interface(tapInterface.get());
	
	// create user thread, running example_main().
	pthread_t appthread;
	int rc = pthread_create(&appthread, NULL, execute_example, NULL);
	
	// start receiving
	while(!abort_execution) {
		char* data;
		// blocks until data available
		unsigned len = tap->read(data);
		demux.demux(data, len, tapInterface.get());
		// do periodic tasks 
		ipstack::System::periodic();
	}
	
	pthread_cancel(appthread);
	pthread_exit(NULL);
	return 0;
}
