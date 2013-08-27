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

/**
 * Only use the provided API of this file if you've build libipstack with the option
 * BUILD_ONLY_ONE_TASK (non-multitask system).
 */

namespace IP {
	// Call this in your mainloop
	void periodic();
	
	/**
	 * Call is_reachable before sending. Only if state is REACHABLE use send().
	 * If you use send() before and it fails because the address is not resolved yet
	 * the buffer will nevertheless be cleared and the packet is lost.
	 * 
	 * If the state is RESOLVE_ADDRESS you may skip sending, do something else and
	 * try again later or use a busy loop (don't forget to call IP::periodic() and to
	 * feed device traffic to the network stack).
	 */
	enum reachable_state {NOT_REACHABLE, REACHABLE, RESOLVE_ADDRESS};
	reachable_state is_reachable(Sendbuffer* sendbuffer);
}