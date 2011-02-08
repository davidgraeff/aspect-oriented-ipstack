#ifndef __ROUTER__
#define __ROUTER__

#include "Interface.h"

namespace ipstack {

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

  Interface* get_interface(int index){
    Interface* interface = head_interface;
    for(int i=0; (i<index) && (interface != 0); i++){
      interface = interface->getNext();
    }
    return interface;
  }
  
};

} // namespace ipstack

#endif /* __ROUTER__ */
