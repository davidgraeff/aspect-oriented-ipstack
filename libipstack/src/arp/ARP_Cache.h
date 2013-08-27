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


#ifndef __ARP_CACHE_H__
#define __ARP_CACHE_H__

namespace ipstack {

class ARP_Cache{
  void foo(); //workaround for AspectC++ Bug: link-once slices require a method BEFORE attributes ;-)
  private:
  //singleton design pattern
  static ARP_Cache inst_;
  ARP_Cache() {} //ctor hidden
  ARP_Cache(const ARP_Cache&); //copy ctor hidden
  ARP_Cache& operator=(ARP_Cache const&); // assign op. hidden
  //~ARP_Cache(); // dtor hidden: in CiAO ok, in linux-user-space not allowed!
  
  void clear() {} //to be implemented by extension aspects
  
  public:
  static ARP_Cache& Inst() { return inst_; } //get singleton
};

} // namespace ipstack

#endif /* __ARP_CACHE_H__ */
