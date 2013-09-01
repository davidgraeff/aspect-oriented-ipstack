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

namespace ipstack {
	/**
	 * The Management_Task is a singleton and required to get access to all
	 * management sockets.
	 * 
	 * For adding a management socket you would do this:
	 * 1) Slice in a Pointer_to_socket:get_your_socket() function.
	 * 2) Slice in a Pointer_to_socket for the above function.
	 * 3) Use initialize_management_sockets to create the socket as "static object".
	 *    And set the Pointer_to_socket class member accordingly.
	 */
	class Management_Task {
		private:
		//singleton design pattern
		static Management_Task inst_;
		Management_Task() {initialize_management_sockets();} //ctor hidden
		Management_Task(const Management_Task&); //copy ctor hidden
		Management_Task& operator=(Management_Task const&); // assign op. hidden

		public:
		static Management_Task& Inst() { return inst_; } //get singleton
		
		/**
		 * Influenced by aspects. 
		 */
		void initialize_management_sockets() {}

	};
} // namespace ipstack
