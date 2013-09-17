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
// Copyright (C) 2012 David Gräff

#pragma once

#include "ReceiveBuffer.h"
#include "util/ipstack_inttypes.h"
#include "memory_management/SocketMemory.h"
#include <string.h> //for memcpy

namespace ipstack
{

	/**
	* Smart pointer for ReceiveBuffer. Will free the ReceiveBuffer, if pointer class goes out of scope.
	* If you want the object to survive the scope, either copy this smart pointer (c++ copy operator, c++11 move operator)
	* or extract the ReceiveBuffer pointer with "takeOwnership".
	* 
	* Please notice: There is no copy semantic for this smart pointer. If you do a copy or assignment
	* internally always a move is executed and the original smart pointer is invalid after that. Example:
	* 
	* SmartReceiveBufferPtr sb = socket->receive();
	* if (sb==0) {abort();}
	* SmartReceiveBufferPtr other_sb(sb);
	* // sb is 0 now!
	* SmartReceiveBufferPtr other_sb2 = other_sb;
	* // other_sb is 0 now!
	*/
	class SmartReceiveBufferPtr : public ReceiveBufferPrivate
	{
		public:
			explicit SmartReceiveBufferPtr(ReceiveBuffer* b, SocketMemory* socket) : pData(b), socket(socket) {}
			~SmartReceiveBufferPtr() { if (pData) socket->free(pData); }
			inline ReceiveBuffer& operator* () { return *pData; }
			inline ReceiveBuffer* operator-> () { return pData; }
			inline ReceiveBuffer* receivebuffer_pointer() { return pData; }
			bool operator==(SmartReceiveBufferPtr& rhs) const {return (pData==rhs.pData);}
			
			// Copy constructor (we use it as move operator and modify the original smart pointer!)
			SmartReceiveBufferPtr(const SmartReceiveBufferPtr& sp)
			{
				pData = ((SmartReceiveBufferPtr&*)sp).takeOwnership();
				socket = sp.socket;
			}
			
			// Assignment operator
			SmartReceiveBufferPtr& operator= (const SmartReceiveBufferPtr& sp)
			{
				if (this != &sp) // Avoid self assignment
				{
					pData = ((SmartReceiveBufferPtr&*)sp).takeOwnership();
					socket = sp.socket;
				}
				return *this;
			}
			
			ReceiveBuffer* takeOwnership() {
				ReceiveBuffer* copy = pData;
				pData = 0; // this instance of the smart pointer is invalid now and will not free ReceiveBuffer anymore!
				return copy;
			}
		protected:
			ReceiveBuffer* pData;
			SocketMemory* socket;

	};

} // namespace ipstack

