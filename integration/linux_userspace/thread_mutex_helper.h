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
#include <pthread.h>

class Mutex
{
	pthread_mutex_t m;
public:
	explicit Mutex() { pthread_mutex_init(&m,NULL);}
	void lock() {  pthread_mutex_lock(&m); }
	void release() {  pthread_mutex_unlock(&m); }
	~Mutex() { pthread_mutex_unlock(&m); pthread_mutex_destroy(&m); }
};

class Lock
{
	Mutex* m;
	public:
	explicit Lock(Mutex* m) : m(m) { m->lock(); }
	~Lock() { m->release(); }
};