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

#pragma once
#include "router/Interface.h"

namespace ipstack {
	/**
	 * Public demux API: Route incoming traffic of your hardware to the demux(...)
	 * method.
	 */
	class ReceiveBuffer;
	class Demux {  
	public:
		static Demux& Inst() { return inst_; } //get singleton
		void demux(const void* data, unsigned len, Interface* interface);
		
		/**
		* Call this if the receiving queue is full
		*/
		void error_receiving_queue_full(ReceiveBuffer* buffer) {
			//Aspects to be weaved here!
		}
		/**
		* Call this if the received packet does not fit into one of the available memory blocks
		*/
		void error_receiving_memory_too_small(ReceiveBuffer* buffer) {
			//Aspects to be weaved here!
		}
	private:
		//singleton design pattern
		static Demux inst_;
		Demux() {} //ctor hidden
		Demux(const Demux&); //copy ctor hidden
		Demux& operator=(Demux const&); // assign op. hidden
	};

} // namespace ipstack
