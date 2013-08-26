#include "IPv6iphc.h"
#include "cfAttribs.h"

namespace ipstack {
	// Hop count
	#ifdef ipstack_ipv6_hc_hopcount_uncompressed
	enum {HOPLIMIT_SIZE = 1};
	#else
	enum {HOPLIMIT_SIZE = 0};
	#endif
	
	#define TF (data[0] & 0x18)
	#define NH (data[0] & 0x04)
	#define HLIM (data[0] & 0x03)
	#define CID (data[1] & 0x80)
	#define SAC (data[1] & 0x40)
	#define SAM (data[1] & 0x30)
	#define M (data[1] & 0x08)
	#define DAC (data[1] & 0x04)
	#define DAM (data[1] & 0x03)
	
	// init to an arbitrary number
	UInt16 IPv6hc_Packet::datagram_tag_counter = 1;
	
void IPv6hc_Packet::writeFragmentHeader(UInt16 size)
{
	fragmentheader[0] = 0xC0 | (0x0700 & size); // 11000xxx | 5 bits of size are ignored and then 3 bits are in fragmentheader[0]
	fragmentheader[1] = (UInt8) 0x00FF & size; // the remaining size is in fragmentheader[1]
	datagram_tag = datagram_tag_counter++;
}
void IPv6hc_Packet::setUpHeader()
{
	data[0] = 0x60; // no flow header, no traffic class, always next header, next hop uncompressed
#ifdef ipstack_ipv6_hc_hopcount_255
	data[0] |= 3;
#endif
#ifdef ipstack_ipv6_hc_hopcount_64
	data[0] |= 2;
#endif
#ifdef ipstack_ipv6_hc_hopcount_1
	data[0] |= 1;
#endif
	data[1] = 0;
}
UInt16 IPv6hc_Packet::get_payload_len()
{
	return (fragmentheader[0] & 0x07 << 8) | fragmentheader[1];
}
void IPv6hc_Packet::set_payload_len(UInt16 len)
{
	writeFragmentHeader(len);
}
UInt8* IPv6hc_Packet::get_payload()
{
	return data + calculateHeaderSize();
}
UInt8 IPv6hc_Packet::getWriteHeaderSize()   // excluding src+dest addr sizes
{
	return 1 + HOPLIMIT_SIZE + IPv6hc_Packet::HC_HEADERSIZE_WITHOUT_FRAG_HEADER + IPv6hc_Packet::HC_FRAG_HEADER;
}
UInt8 IPv6hc_Packet::calculateHCHeaderSizeWithoutFragmentSize()
{
	UInt8 offset = 2;
	switch (CID) { // context identifier
		case 1:
			offset += 1;
			break;
		default:
			break;
	};
	return offset;
}
UInt8 IPv6hc_Packet::calculateHeaderSize()
{
	UInt8 offset = 4;
	switch (TF) {
		case 0:
			offset += 4;
			break; // Traffic class + flow label
		case 1:
			offset += 3;
			break; // Part of traffic class + flow label
		case 2:
			offset += 1;
			break; // traffic class
		default:
			break;
	};
	switch (NH) { // next header
		case 0:
			offset += 1;
			break;
		default:
			break;
	};
	switch (HLIM) { // hop limit
		case 0:
			offset += 1;
			break;
		default:
			break;
	};
	switch (CID) { // context identifier
		case 1:
			offset += 1;
			break;
		default:
			break;
	};
	switch (SAC) { // source address
		case 0: {
				switch (SAM) {
					case 0:
						offset += 16;
						break; // full source address
					case 1:
						offset += 8;
						break; //
					case 2:
						offset += 2;
						break; //
					default:
						break;
				}
			};
		case 1: {
				switch (SAM) {
					case 1:
						offset += 8;
						break; // f
					case 2:
						offset += 2;
						break; //
					default:
						break;
				}
			};
		default:
			break;
	};
	switch (M) {
		case 0: { // no multicast
				switch (DAC) { // dest address
					case 0: {
							switch (DAM) {
								case 0:
									offset += 16;
									break; // full source address
								case 1:
									offset += 8;
									break;
								case 2:
									offset += 2;
									break;
								default:
									break;
							}
						};
					case 1: {
							switch (DAM) {
								case 1:
									offset += 8;
									break; //
								case 2:
									offset += 2;
									break; //
								default:
									break;
							}
						};
					default:
						break;
				};
			}
		case 1: { // multicast
				switch (DAC) { // dest address
					case 0: {
							switch (DAM) {
								case 0:
									offset += 16;
									break; // full source address
								case 1:
									offset += 6;
									break; //
								case 2:
									offset += 4;
									break; //
								case 3:
									offset += 1;
									break; //
								default:
									break;
							}
						};
					case 1: {
							switch (DAM) {
								case 0:
									offset += 6;
									break; //
								default:
									break;
							}
						};
					default:
						break;
				};
			}
		default:
			break;
	};


	return offset;
}
UInt8 IPv6hc_Packet::get_hoplimit()
{
	switch (HLIM) { // hop limit
		case 0: { // carry in-line
				UInt8 offset = calculateHCHeaderSizeWithoutFragmentSize();
				switch (TF) {
					case 0:
						offset += 4;
						break; // Traffic class + flow label
					case 1:
						offset += 3;
						break; // Part of traffic class + flow label
					case 2:
						offset += 1;
						break; // traffic class
					default:
						break;
				};
				switch (NH) { // next header
					case 0:
						offset += 1;
						break;
					default:
						break;
				};
				return data[offset];

			}
		case 1:
			return 1;
		case 2:
			return 64;
		case 3:
			return 255;
		default:
			return 0;
	};
}
void IPv6hc_Packet::set_hoplimit(UInt8 t)
{
#ifdef ipstack_ipv6_hc_hopcount_uncompressed
	UInt8 offset = HC_HEADERSIZE_WITHOUT_FRAG_HEADER;
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	// OFFSET FÜR NEXT HEADER: 1
	offset += 1;
	data[offset] = t;
#endif
}
UInt8 IPv6hc_Packet::get_nextheader()
{
	switch (NH) { // next header
		case 0: { // carry in-line
				UInt8 offset = calculateHCHeaderSizeWithoutFragmentSize();
				switch (TF) {
					case 0:
						offset += 4;
						break; // Traffic class + flow label
					case 1:
						offset += 3;
						break; // Part of traffic class + flow label
					case 2:
						offset += 1;
						break; // traffic class
					default:
						break;
				};
				return data[offset];

			}
		default:
			return 0; // FAILURE
	};
}
void IPv6hc_Packet::set_nextheader(UInt8 i)
{
	UInt8 offset = HC_HEADERSIZE_WITHOUT_FRAG_HEADER;
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	data[offset] = i;
}
ipv6addr IPv6hc_Packet::get_src_ipaddr()
{
	static ipv6addr mem; // this has to be static,
	// because the memory position will be used for the uncompressed src address
	UInt8 offset = calculateHCHeaderSizeWithoutFragmentSize();
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	// OFFSET FÜR NEXT HEADER: 1
	offset += 1;
	// OFFSET FÜR HOPLIMIT: 1
	offset += HOPLIMIT_SIZE;
	// Generate the ip
	return read_compressed_ipaddr(mem, &data[offset], data[1] & 0x30);
}
void IPv6hc_Packet::set_src_ipaddr(const ipv6addr& addr)
{
	UInt8 offset = HC_HEADERSIZE_WITHOUT_FRAG_HEADER;
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	// OFFSET FÜR NEXT HEADER: 1
	offset += 1;
	// OFFSET FÜR HOPLIMIT: 1
	offset += HOPLIMIT_SIZE;
	write_compressed_src_ipaddr(addr, &data[offset], data[1]);
}
ipv6addr IPv6hc_Packet::get_dst_ipaddr()
{
	static ipv6addr mem; // this has to be static,
	// because the memory position will be used for the uncompressed dest address
	UInt8 offset = calculateHCHeaderSizeWithoutFragmentSize();
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	// OFFSET FÜR NEXT HEADER: 1
	offset += 1;
	// OFFSET FÜR HOPLIMIT: 1
	offset += HOPLIMIT_SIZE;
	// OFFSET FOR SRC ADDR
	offset += getWriteSrcHeaderSize();
	// Is Multicast?
	if ((data[1] & 0x08) != 0)
		return read_compressed_multicast_dest_ipaddr(mem, &data[offset], data[1] & 0x03);
	else
		return read_compressed_ipaddr(mem, &data[offset], data[1] & 0x03);
}
void IPv6hc_Packet::set_dst_ipaddr(const ipv6addr& addr)
{
	UInt8 offset = HC_HEADERSIZE_WITHOUT_FRAG_HEADER;
	// OFFSET FOR TRAFFIC CLASS AND FLOW LABEL: 0
	// OFFSET FÜR NEXT HEADER: 1
	offset += 1;
	// OFFSET FÜR HOPLIMIT: 1
	offset += HOPLIMIT_SIZE;
	// OFFSET FOR SRC ADDR
	offset += getWriteSrcHeaderSize();
	// Generate the ip
	if (IPV6AddressScope::getIPv6AddressScope(addr) == IPV6AddressScope::IPV6_SCOPE_MULTICAST) {
		write_compressed_multicast_dst_ipaddr(addr, &data[offset], data[1]);
	} else {
		write_compressed_dst_ipaddr(addr, &data[offset], data[1]);
	}
}
ipv6addr IPv6hc_Packet::read_compressed_ipaddr(ipv6addr& mem, UInt8* readPtr, UInt8 flagbyte)
{
	switch (flagbyte) {
		case 00: { // full addr inline: everything super
				ipv6addr* tmpAddr = (ipv6addr*)&readPtr;
				return *tmpAddr;
			}
		case 01: { // 64bit
				get_unspecified_ipv6_address(mem);
				// link local prefix
				mem.ipaddrB8[0] = 0xfe;
				mem.ipaddrB8[1] = 0x80;
				// rest (8 bytes) is from received packet
				memcpy(&mem.ipaddrB8[8], readPtr, 8);
				return mem;
			}
		case 10: { // 16bit
				get_unspecified_ipv6_address(mem);
				// link local prefix
				mem.ipaddrB8[0] = 0xfe;
				mem.ipaddrB8[1] = 0x80;
				mem.ipaddrB8[11] = 0xff;
				mem.ipaddrB8[12] = 0xfe;
				// rest (2 bytes) is from received packet
				mem.ipaddrB8[14] = readPtr[0];
				mem.ipaddrB8[15] = readPtr[1];
				return mem;
			}
		case 11: { // Get ip from upper protocol
				get_unspecified_ipv6_address(mem);
				mem.ipaddrB8[0] = 0xfe;
				mem.ipaddrB8[1] = 0x80;
				get_dest_ip_from_upper_protocol_addr(mem);
				return mem;
			}

		default: {
				get_unspecified_ipv6_address(mem);
				return mem;
			}
	};
}
ipv6addr IPv6hc_Packet::read_compressed_multicast_dest_ipaddr(ipv6addr& mem, UInt8* readPtr, UInt8 flagbyte)
{
	switch (flagbyte) {
		case 00: { // full addr inline: everything super
				ipv6addr* tmpAddr = (ipv6addr*)&readPtr;
				return *tmpAddr;
			}
		case 01: { // 48 bits ffXX::00XX:XXXX:XXXX.
				get_unspecified_ipv6_address(mem);
				mem.ipaddrB8[0] = 0xff;
				mem.ipaddrB8[1] = readPtr[0];
				mem.ipaddrB8[11] = readPtr[1];
				mem.ipaddrB8[12] = readPtr[2];
				mem.ipaddrB8[13] = readPtr[3];
				mem.ipaddrB8[14] = readPtr[4];
				mem.ipaddrB8[15] = readPtr[5];
				return mem;
			}
		case 10: { // 32 bits ffXX::00XX:XXXX.
				get_unspecified_ipv6_address(mem);
				mem.ipaddrB8[0] = 0xff;
				mem.ipaddrB8[1] = readPtr[0];
				mem.ipaddrB8[13] = readPtr[1];
				mem.ipaddrB8[14] = readPtr[2];
				mem.ipaddrB8[15] = readPtr[3];
				return mem;
			}
		case 11: { // 8 bits ff02::00XX.
				get_unspecified_ipv6_address(mem);
				mem.ipaddrB8[0] = 0xff;
				mem.ipaddrB8[1] = 0x02;
				mem.ipaddrB8[15] = readPtr[0];
				return mem;
			}

		default: {
				get_unspecified_ipv6_address(mem);
				return mem;
			}
	};
}
void IPv6hc_Packet::write_compressed_src_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte)
{
	// If no aspect intercepts, we do not compress the src addr
	memcpy(writePtr, addr.ipaddrB8, 16);
}
void IPv6hc_Packet::write_compressed_dst_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte)
{
	// If no aspect intercepts, we do not compress the dest addr
	memcpy(writePtr, addr.ipaddrB8, 16);
}
void IPv6hc_Packet::write_compressed_multicast_dst_ipaddr(const ipv6addr& addr, UInt8* writePtr, UInt8& flagbyte)
{
	flagbyte |= 0x08; // is a multicast address
	// If no aspect intercepts, we do not compress the multicast dest addr
	memcpy(writePtr, addr.ipaddrB8, 16);
}
}
