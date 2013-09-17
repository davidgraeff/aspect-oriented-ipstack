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
// Copyright (C) 2011 Christoph Borchert, 2013 David Gräff

#pragma once

namespace ipstack {
	class Interface;
	
	/**
	 * Public API for the router. Add your interface here,
	 * to be able to send data with libipstack.
	 * Router is a singleton, get the instance by Router::Inst().
	 */
	class Router {
	public:
		/// Get singleton
		static Router& Inst() { return inst_; } 

		/// Get interface with number @index
		Interface* get_interface(int index);
		
		/// Add hardware driver interface
		void add_interface(Interface* interface);
	private:
		Router() {} //ctor hidden
		Router(const Router&); //copy ctor hidden
		Router& operator=(Router const&); // assign op. hidden
		static Router inst_;
		Interface* head_interface; //Interfaces (linked list) head pointer
	};
} // namespace ipstack

