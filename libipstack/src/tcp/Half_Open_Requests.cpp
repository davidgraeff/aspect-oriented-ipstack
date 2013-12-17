#include "Half_Open_Requests.h"
#include "tcp/TCP_Segment.h"
#include "ip/SendbufferIP.h"

namespace ipstack
{
	void Half_Open_Requests::respond(ReceiveBuffer& b) {
		TCP_Segment* incoming_segment = static_cast<TCP_Segment*>(b.get_payload_data());
		uint_fast16_t payload_len = b.get_payload_size();
		SendBuffer* sendbuffer = SendbufferIP::requestIPBuffer(*this, *this, payload_len, *b);

		TCP_Segment* segment = (TCP_Segment*)sendbuffer->getDataPointer();
		segment->set_dport(incoming_segment->get_sport());
		segment->set_sport(incoming_segment->get_dport());
		segment->set_header_len(TCP_Segment::TCP_MIN_HEADER_SIZE / 4);
		segment->set_window(0);
		segment->set_urgend_ptr(0);
		segment->set_checksum(0);
		// Checksum is calculated by an aspect
		// segment->set_checksum(InternetChecksum::compute(ipv4_tcp_rst_packet.get_IPv4_Packet(), 0));

		// If the incoming segment has an ACK field, the reset takes its
		// sequence number from the ACK field of the segment, otherwise the
		// reset has sequence number zero and the ACK field is set to the sum
		// of the sequence number and segment length of the incoming segment.
		// The connection remains in the CLOSED state. (rfc 793, page 36)

		if (incoming_segment->has_ACK()) {
			segment->set_seqnum(incoming_segment->get_acknum());
			segment->set_flags(TCP_Segment::RST_FLAG);
			segment->set_acknum(0);
		} else {
			if (incoming_segment->has_SYN() || incoming_segment->has_FIN()) {
				//SYN, FIN consumes one sequence number
				payload_len++;
			}
			segment->set_seqnum(0);
			segment->set_flags(TCP_Segment::RST_FLAG | TCP_Segment::ACK_FLAG);
			segment->set_acknum(incoming_segment->get_seqnum() + payload_len);
		}

		sendbuffer->writtenToDataPointer(TCP_Segment::TCP_MIN_HEADER_SIZE);
		sendbuffer->send();
	}
}
