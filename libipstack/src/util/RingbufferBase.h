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


#ifndef __RINGBUFFER_BASE_H__
#define __RINGBUFFER_BASE_H__

namespace ipstack {

class PolymorphRingbufferBase {
public:
  virtual void put(void* val) volatile = 0;
  virtual void* get() volatile = 0;
  virtual bool isFull() volatile const = 0;
};

class EmptyRingbufferBase {};

} //namespace ipstack

#endif // __RINGBUFFER_BASE_H__


