// This file is part of CiAO/IP.
//
// CiAO/IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CiAO/IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CiAO/IP.  If not, see <http://www.gnu.org/licenses/>.
// 
// Copyright (C) 2011 Christoph Borchert


#include "Router.h"
#include "Interface.h"

namespace ipstack {

Router Router::inst_; //create singleton instance

Interface* Router::get_interface(int index)
{
	Interface* interface = head_interface;
	for (int i = 0; (i < index) && (interface != 0); i++) {
		interface = interface->getNext();
	}
	return interface;
}
} // namespace ipstack

