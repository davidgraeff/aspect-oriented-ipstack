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
/**
 * Purpose: Add a next pointer to a class so that sockets etc can be organised in a linked-list fashion
 * 
 * Implementation detail: We use template specialisation to either 
 * have a linked list with initialized head to the first constructed object in the
 * list or a linked list where you have to call setHead() manually.
 */

namespace ipstack {
		
	template <class T>
	class LinkedListPrivate {
	protected:
		T* next;
		static T* head;
	public:
		LinkedListPrivate() : next(0) {}
		inline T* getNext() { return next; }
		inline void setNext(T* sock) {
			next = sock;
		}
		static T* getHead() {
			return head;
		}
		static void setHead(T* h) { head = h; }
	};

	/**
	 * Case: No head initialized
	 */
	template <class T, bool initHead = false>
	class LinkedList : public LinkedListPrivate<T> {
	public:
		LinkedList() {}
	};

	/**
	 * Case: Head initialized with first object in the list
	 */
	template <class T>
	class LinkedList<T, true> : public LinkedListPrivate<T> {
	public:
		LinkedList() {
			if (!LinkedListPrivate<T>::getHead())
				this->setHead((T*)this);
		}
	};

	template <class T>
	T* LinkedListPrivate<T>::head = 0;

} // namespace ipstack
