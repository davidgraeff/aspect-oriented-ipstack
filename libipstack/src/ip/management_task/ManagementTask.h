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
#include "memory_management/MemoryInterface.h"

namespace ipstack {
	/**
	 * The Management_Task is a singleton and required to get access to all
	 * management sockets.
	 * 
	 * If you have a multitask system, create a task and call run(). You should
	 * implement block() for performance reasons that should block the task while
	 * no new packet is received. If you are using a single task system, you do not
	 * have to interact with this class because runIteration is called from
	 * IP::periodic().
	 * 
	 * For adding a management socket you would do this:
	 * 1) Slice in a <MEMORY& get_mem_your_socket()> function where yoo
	 *    create your socket memory as static variable and return it.
	 * 2) Slice in a <YOUR_SOCKET& get_your_socket()> function where yoo
	 *    create your socket as static variable  and call get_mem_your_socket() to get the sockets memory.
	 * 
	 *    Example:
	 *    static MemoryInterface& get_mem_icmpv4() { static ManagementSocketMemory mem; return mem; }
	 *    ICMPv4_Socket& get_socket_icmpv4() { static ICMPv4_Socket socket(get_mem_icmpv4()); return socket; }
	 */
	class Management_Task {
		private:
		//singleton design pattern
		static Management_Task inst_;
		Management_Task() {} //ctor hidden
		Management_Task(const Management_Task&); //copy ctor hidden
		Management_Task& operator=(Management_Task const&); // assign op. hidden

		public:
		static Management_Task& Inst() { return inst_; } //get singleton
		
		/**
		 * Entry point for this task. It executes runIteration in a loop, calling
		 * block() after each iteration.
		 */
		void run();
		void block();
		void runIteration();
	};
} // namespace ipstack
