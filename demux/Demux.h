#ifndef __DEMUX__
#define __DEMUX__

#include "../router/Interface.h"

namespace ipstack {

class Demux {  
  private:
  //singleton design pattern
  static Demux inst_;
  Demux() {} //ctor hidden
  Demux(const Demux&); //copy ctor hidden
  Demux& operator=(Demux const&); // assign op. hidden
  //~Demux(); // dtor hidden: in CiAO ok, in linux-user-space not allowed!
  
  //the main demux function
  void demux(const void* data, unsigned len, Interface* interface);
  
  public:
  static Demux& Inst() { return inst_; } //get singleton
};

} // namespace ipstack

#endif /* __DEMUX__ */
