
		
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
#include "memory_management/Ringbuffer.h"

/**
 * Interface for beeing used in a socket class
 * to include memory management.
 */
class SocketMemory
{
protected:
	MemoryInterface* mempool;
	Packetbuffer* packetbuffer;
public:
	SocketMemory(MemoryInterface* m, Packetbuffer* pb=0) : mempool(m), packetbuffer(pb) {}
	
	// Packetbuffer
	inline Packetbuffer* get_packetbuffer() { return packetbuffer; }
	inline bool is_packetbuffer_full() {return packetbuffer->isFull();}
	inline void set_packetbuffer(Packetbuffer* pb) { packetbuffer = pb; }
	// Memory pool
	inline void set_Mempool(MemoryInterface* m) { mempool = m; }
	inline MemoryInterface* get_Mempool() { return mempool; }
};
