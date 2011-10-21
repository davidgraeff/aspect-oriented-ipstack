#ifndef __INTERFACE__
#define __INTERFACE__

namespace ipstack {

class Interface {
  private:
  Interface* next_interface; //linked list
  //IPv4 Attributes to be sliced in by IPv4-Aspect
  
  public:
  enum { ETHERNET = 1,
         LOOPBACK = 2,
         SLIP = 3 };
  
  Interface* getNext() { return next_interface; }
  
  //these functions have to be delegated to network device abstraction
  // -> performed by aspect Interface.ah
  const char* getName() { return 0; }
  unsigned char getType() { return 0; }
  const unsigned char* getAddress() { return 0; }
  unsigned getMTU() { return 0; }
  
  bool hasTransmitterHardwareChecksumming() { return false; }
  bool hasReceiverHardwareChecksumming() { return false; }
  
  void send(const void* frame, unsigned framesize) {}
  void send(const void* data, unsigned datasize, bool tcp, unsigned ip_hdl) {}
  bool hasBeenSent(const void* frame){ return true; }

};

} // namespace ipstack

#endif /* INTERFACE */

