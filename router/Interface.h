#ifndef __INTERFACE__
#define __INTERFACE__

namespace hw {
namespace hal {
class NetworkDevice; //Forward declaration solves include-cycle problem
}}
using hw::hal::NetworkDevice;


namespace ipstack {

class Interface{
  private:
  Interface* next_interface; //linked list
  NetworkDevice* device; //the device driver (for delegation)
  //IPv4 Attributes to be sliced in by IPv4-Aspect
  
  public:
  enum { ETHERNET = 1,
         LOOPBACK = 2,
         SLIP = 3 };
  
  Interface* getNext() { return next_interface; }
  void setNetworkDevice(NetworkDevice* dev){ device = dev; }
  
  //delegation to NetworkDevice
  const char* getName();
  unsigned char getType();
  const unsigned char* getAddress();
  unsigned getMTU();
  
  bool hasTransmitterHardwareChecksumming();
  bool hasReceiverHardwareChecksumming();
  
  void send(const void* frame, unsigned framesize);
  void send(const void* data, unsigned datasize, bool tcp, unsigned ip_hdl);
  bool hasBeenSent(const void* frame);

};

} // namespace ipstack

#endif /* INTERFACE */

