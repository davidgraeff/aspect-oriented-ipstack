#include "Interface.h"
#include "hw/hal/NetworkDevice.h"

namespace ipstack{

//delegation to NetworkDevice
const char* Interface::getName(){
  return device->getName();
}

unsigned char Interface::getType(){
  return device->getType();
}

unsigned Interface::getMTU(){
  return device->getMTU();
}

const unsigned char* Interface::getAddress(){
  return device->getAddress();
}

bool Interface::hasTransmitterHardwareChecksumming(){
  return device->hasTransmitterHardwareChecksumming();
}

bool Interface::hasReceiverHardwareChecksumming(){
  return device->hasReceiverHardwareChecksumming();
}

void Interface::send(const void* frame, unsigned framesize){
  device->send(frame, framesize);
}

void Interface::send(const void* frame, unsigned framesize, bool tcp, unsigned ip_hdl){
  device->send(frame, framesize, tcp, ip_hdl);
}
  
bool Interface::hasBeenSent(const void* frame){
  return device->hasBeenSent(frame);
}

} // namespace ipstack
