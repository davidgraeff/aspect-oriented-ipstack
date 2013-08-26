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
// Copyright (C) 2012 David Gräff

#pragma once

#include "router/Interface.h"
#include "ipv6/IPv6AddressUtilities.h"
#include "util/types.h"
namespace ipstack
{
/**
 * 
 */
class MLDMessages
{
	struct mldv1_listener_report {
		UInt8 type; // 131
		UInt8 code; // 0
		UInt16 checksum;
		UInt16 maximum_response_delay;
		UInt16 reserved;
		ipv6addr addr;
	} __attribute__((packed));
	
	struct mldv1_listener_done {
		UInt8 type; // 132
		UInt8 code; // 0
		UInt16 checksum;
		UInt16 maximum_response_delay;
		UInt16 reserved;
		ipv6addr addr;
	} __attribute__((packed));
	
	struct mldv2_listener_report_message_record {
		UInt8 record_type;
		UInt8 aux_data_len; // 0
		UInt16 number_of_sources;
		ipv6addr multicast_addr;
		ipv6addr src_addresses[];
	} __attribute__((packed));
	
	struct mldv2_listener_report_message {
		UInt8 type; // 143
		UInt8 code; // 0
		UInt16 checksum;
		UInt16 reserved;
		union { UInt16 multicast_address_records; UInt8 multicast_address_records8[2];};
		mldv2_listener_report_message_record records[];
	} __attribute__((packed));
	
	struct mld_multicast_listener_query {
		UInt8 type; // 130
		UInt8 code; // 0
		UInt16 checksum;
		UInt16 maximum_response_delay;
		UInt16 reserved;
		ipv6addr addr;
	} __attribute__((packed));
	
	// mld version1: send this message if you want to register to a multicast group
	static void write_mldv1_listener_report(char* data, const ipstack::ipv6addr& multicast_addr) {
		mldv1_listener_report* d= (mldv1_listener_report*)data;
		d->type = 131;
		d->code = 0;
		d->checksum = 0;
		d->maximum_response_delay = 0;
		d->reserved = 0;
		copy_ipv6_addr(multicast_addr, d->addr);
	}
	
	// mld version1: send this message if you want to unregister from a multicast group
	static void write_mldv1_listener_done(char* data, const ipstack::ipv6addr& multicast_addr) {
		mldv1_listener_done* d= (mldv1_listener_done*)data;
		d->type = 132;
		d->code = 0;
		d->checksum = 0;
		d->maximum_response_delay = 0;
		d->reserved = 0;
		copy_ipv6_addr(multicast_addr, d->addr);
	}
	
	enum mldv2_listener_report_record_type {mldv2_CHANGE_TO_INCLUDE_MODE=3, mldv2_CHANGE_TO_EXCLUDE_MODE=4 };
	static void write_mldv2_listener_report_message(char* data, const ipstack::ipv6addr& multicast_addr, mldv2_listener_report_record_type rtype ) {
		mldv2_listener_report_message* d= (mldv2_listener_report_message*)data;
		d->type = 143;
		d->code = 0;
		d->checksum = 0;
		d->reserved = 0;
		// set manually to avoid byte-order conflicts
		d->multicast_address_records8[0] = 0;
		d->multicast_address_records8[1] = 1;
		mldv2_listener_report_message_record* r = (mldv2_listener_report_message_record*)d->records;
		r->record_type = rtype;
		r->aux_data_len = 0;
		r->number_of_sources = 0;
		copy_ipv6_addr(multicast_addr, r->multicast_addr);
	}
	static const UInt8 mldv2_listener_report_message_size() { return sizeof(mldv2_listener_report_message)+sizeof(mldv2_listener_report_message_record); }
};

} //namespace ipstack
