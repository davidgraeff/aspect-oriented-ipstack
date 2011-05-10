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
