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
 * BUILD_WITH_ASPECTLESS_INTERFACE and not with BUILD_ONLY_ONE_TASK. Multitask
 * context switches and IRQs make it necessary to provide a mechanism for exclusive access to
 * some code paths. Implement the following methods for protecting those paths.
 */

namespace IP {
	/**
	 * Implement this method.
	 * Expectially called while doing memory operations like insert/delete.
	 */
	void lock_multitask_access();
	
	/**
	 * Implement this method.
	 */
	void unlock_multitask_access();
}