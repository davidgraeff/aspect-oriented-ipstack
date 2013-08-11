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


#ifndef __ROUTER__
#define __ROUTER__

namespace ipstack {
class Interface;
class Router {
  private:
  //singleton design pattern
  static Router inst_;
  Router() {} //ctor hidden
  Router(const Router&); //copy ctor hidden
  Router& operator=(Router const&); // assign op. hidden
  //~Router(); // dtor hidden: in CiAO ok, in linux-user-space not allowed!

  //Interfaces (linked list) head pointer
  Interface* head_interface;
  
  public:
  static Router& Inst() { return inst_; } //get singleton

  Interface* get_interface(int index);
  
};

} // namespace ipstack

#endif /* __ROUTER__ */
