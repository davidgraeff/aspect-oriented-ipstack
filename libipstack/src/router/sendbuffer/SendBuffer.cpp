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
// Copyright (C) 2012-2013 David Gräff

#pragma once

#include "SendBuffer.h"
namespace ipstack
{
		static SendBuffer* SendBuffer::createInstance(MemoryInterface* mem, uint_fast16_t requestedSize, Interface* interface) {
			SendBuffer* r = (SendBuffer*)mem->alloc(requestedSize + sizeof(SendBuffer));
			if (!r)
				return 0;
			r->m_memsize = requestedSize;
			r->m_state = WritingState;
			r->m_interface = interface;
			r->initStartPointer();
			return r;
		}
		
		void SendBuffer::initStartPointer() {
			data = getDataStart();
		}

		void* SendBuffer::getDataPointer() {
			return data;
		}

		void SendBuffer::writtenToDataPointer(uint_fast16_t length) {
			data = (char*)data + length;
		}

		void SendBuffer::write(const void* newdata, uint_fast16_t length) {
			uint_fast16_t availableLength = getRemainingSize();
			if (availableLength < length)
				length = availableLength;

			memcpy(data, newdata, length);
			data = (char*)data + length;
		}
		
		void SendBuffer::recycle() {
			// block until buffer has been send by the hardware
			if (m_state==SendBuffer::TransmittedState)
				while(!m_interface->hasBeenSent(getDataStart())) {}
			// reset the state to WriteState
			m_state = WritingState;
		}

		uint_fast16_t SendBuffer::getRemainingSize() {
			return m_memsize - ((char*) data - (char*) getDataStart());
		}
		uint_fast16_t SendBuffer::getSize() {
			return m_memsize;
		}
		
		void SendBuffer::setInterface(Interface* i) { m_interface = i; }

		explicit SendBuffer::SendBuffer() {} // private constructor
		SendBuffer::SendBuffer(const SendBuffer& s) {} // private copy constructor
};

} // namespace ipstack
