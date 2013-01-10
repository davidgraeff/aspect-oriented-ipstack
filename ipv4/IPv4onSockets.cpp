#include "IPv4onSockets.h"
#include "../router/Interface.h"
#include "../router/Router.h"
#include "IPv4.h"

namespace ipstack {
void IPV4::resolveRoute()
{
	Router& router = Router::Inst(); //short name
	interface = router.ipv4_find_route(dst_ipv4_addr);
	using_gateway = false;
	if (interface != 0)
		return;

	//use default gateway
	interface = router.ipv4_find_route(router.ipv4_get_gateway_addr());
	using_gateway = true;
}
void IPV4::setupHeader(IPv4_Packet* packet, unsigned int datasize)
{
	packet->set_ihl(IPv4_Packet::IPV4_MIN_HEADER_SIZE / 4);
	packet->set_version(IPv4_Packet::IPV4_VERSION);
	packet->set_tos(IPv4_Packet::IPV4_DEFAULT_TOS);
	packet->set_total_len(datasize); // incl. IPv4_Packet::IPV4_MIN_HEADER_SIZE
	packet->set_id(id++);
	packet->set_flags(IPv4_Packet::IPV4_DEFAULT_FLAGS);
	packet->set_fragment_offset(IPv4_Packet::IPV4_NO_FRAGMENT_OFFSET);
	packet->set_ttl(IPv4_Packet::IPV4_DEFAULT_TTL);
// 	packet->set_protocol(protocol_id);
	packet->set_src_ipaddr(get_src_addr());
	packet->set_dst_ipaddr(dst_ipv4_addr);
	// do not calculate checksum here, because protocol_id is not known at this moment
}
bool IPV4::hasValidInterface() { return (interface != 0); }
unsigned int IPV4::getSpecificHeaderSize()
{
	return IPv4_Packet::IPV4_MIN_HEADER_SIZE;
}
bool IPV4::hasValidSrcDestAddresses()
{
	return (interface != 0) && (interface->getIPv4Addr() != 0) && (dst_ipv4_addr != 0);
}
IPV4::IPV4() : interface(0), dst_ipv4_addr(0), id(0) {}
UInt32 IPV4::get_src_addr()
{
	if (interface != 0) {
		return interface->getIPv4Addr();
	}
	return 0;
}
void IPV4::set_dst_addr(UInt32 dst)
{
	dst_ipv4_addr = dst;
	resolveRoute();
}
void IPV4::set_dst_addr(UInt8 a, UInt8 b, UInt8 c, UInt8 d)
{
	set_dst_addr(IPv4_Packet::convert_ipv4_addr(a, b, c, d));
}
UInt32 IPV4::get_dst_addr() { return dst_ipv4_addr; }
}
