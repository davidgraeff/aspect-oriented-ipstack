#include "IPv4_Fragment_Reassembly.h";
#include "ipv4/IPv4_Packet.h"
#include "os_integration/Clock.h"
#include <string.h> //for memcpy and memset

namespace ipstack {
	IPv4_Fragment_Reassembly::IPv4_Fragment_Reassembly() {
		
	}
	
	void IPv4_Fragment_Reassembly::clear() {
		
	}
	
	void IPv4_Fragment_Reassembly::update_bitmap(unsigned start, unsigned end){
		start = start & 0xFFFFFFF8; //start at multiple of 8
		
		for(unsigned i=start; i <= end; i+=8){
		unsigned index = i/64;
		unsigned value = (i%64)/8;
		bitmap[index] |= (1 << value);
		}
	}
	
	bool IPv4_Fragment_Reassembly::full_bitmap(){
		//check all bytes in the bitmap except the last one
		for(unsigned i=0; i < (unsigned) ((total_len / (8 * 8)) - 1); i++) {
			if(bitmap[i] != 0xFF) {
				return false;
			}
		}
		//check the last byte now
		uint8_t value = 0;
		unsigned last_byte = total_len & 0xFFFFFF40; //start at multiple of 64
		for(unsigned i=last_byte; i <= total_len; i+=8){
			int bitmap_value = (i%64)/8;
			value |= (1 << bitmap_value);
		}
		if(bitmap[(total_len / (8 * 8))] == value){    
			return true;
		}
		
		return false;
	}
	
	void IPv4_Fragment_Reassembly::add_fragmented_data(ReceiveBuffer* receivebuffer){
		uint64_t curr_time = Clock::now(); //in ticks
		
		bool copy_data = false; //is set if the current packet shall be copied into buffer
		IPv4_Packet* packet = (IPv4_Packet*) receivebuffer->p.ip_packet;
		
		if(curr_time < expired_time){
			// buffer is hot
			
			IPv4_Packet* header = (IPv4_Packet*) buffer;
			
			if( (header->get_src_ipaddr() == packet->get_src_ipaddr()) &&
				(header->get_dst_ipaddr() == packet->get_dst_ipaddr()) &&
				(header->get_id() == packet->get_id()) ) {
					
				//this fragment belongs to buffer
				copy_data = true;
			}
		}
		else{ //TODO: muss fragment_offset nicht gleich 0 sein bei neuem fragmentiertem ip packet?
			//buffer is cold
			//if(header_len != 0){ //DEBUG ONLY
			//  std::cout << "IPv4 Fragment(s) discarded: Timeout!" << std::endl;
			//}
			
			clear_bitmap();
			total_len = 0;
			header_len = packet->get_ihl()*4;
			
			//copy ip header      
			memcpy(buffer, packet, header_len);      
			update_bitmap(0, header_len);
			
			copy_data = true;
		}
		
		if(copy_data == true){
		unsigned data_start = header_len + fragment_offset; //buffer offset in bytes
		unsigned data_len = packet->get_total_len() - header_len; //payload length (for memcpy)
		unsigned data_end = data_start + data_len; //buffer offset in bytes
		
		if(data_end > BUFFERSIZE){
			expired_time = 0; //buffer empty
			//std::cout << "IPv4 Fragment(s) discarded: Too big! ("
				//<<data_end<<" bytes)\n";
			return; //buffer too small!
		}
		
		//TODO: don't use get_data() since it evaluates ihl once again!
		memcpy(&buffer[data_start], packet->get_data(), data_len);
		update_bitmap(data_start, data_end);
		
		if (packet->get_flags() != IPv4_Packet::IPV4_MF_FLAG){ //TODO: flags as a parameter for ipv4_reassemble?
			//This is the last fragment. total length is known
			total_len = data_end;
		}
		
		if (total_len != 0 && full_bitmap()) {
			//reassembly complete :-)          
			//IPv4_Packet* packet = (IPv4_Packet*) buffer;
			packet = (IPv4_Packet*) buffer; //overwrite reference parameter
			packet->set_fragment_offset(0); //no more fragments
			packet->set_flags(0); // no flags
			packet->set_total_len(total_len); //overall length
			//packet->computeChecksum(); //Insert IPv4 header checksum (TODO: don't do that!)
			//demux.ipv4_demux(packet, total_len, interface);
			len = total_len; //overwrite reference parameter
			
			expired_time = 0; //buffer empty
			
			//TODO: call demux.ip_payload(..)
			return;
		}
		
		//restart timeout (until next fragment must arrive)
		expired_time = curr_time + Clock::ms_to_ticks(TIMEOUT * 1000UL);   
	}
}
