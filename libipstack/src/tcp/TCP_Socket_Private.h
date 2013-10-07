// This file is part of Aspect-Oriented-IP.
//
// Aspect-Oriented-IP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Aspect-Oriented-IP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Aspect-Oriented-IP.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2011 Christoph Borchert
#pragma once

#include "memory_management/SocketMemory.h"
#include "demux/DemuxLinkedListContainer.h"

#include "TCP_Segment.h"
#include "tcp_history/TCP_History.h"
#include "tcp_receivebuffer/TCP_ReceiveBuffer.h"
#include "TCP_Config.h"

#include <string.h>

namespace ipstack
{
	class SendBuffer;

	class TCP_Socket_Private
	{
		protected:
			// **************************************************************************
			// *** TCP State ***
			// **************************************************************************
			enum { CLOSED, LISTEN, SYNSENT, SYNRCVD, ESTABLISHED, FINWAIT1, FINWAIT2, CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT } state;
			inline bool isEstablished() { return state == ESTABLISHED; }
			inline bool isCloseWait() { return state == CLOSEWAIT; }
			inline bool isListening() { return state == LISTEN; }
			inline bool isClosed() { return state == CLOSED; }
			inline bool isSynRcvd() { return state == SYNRCVD; }
			inline bool isSynSend() { return state == SYNSENT; }
			bool waiting;
			inline bool waiting_for_input() { return waiting; }

			// *THE* state machine switch
			void input(TCP_Segment* segment, unsigned len);
			void closed(TCP_Segment* segment, unsigned len);
			void closewait(TCP_Segment* segment, unsigned len);
			void closing(TCP_Segment* segment, unsigned len);
			void timewait(TCP_Segment* segment, unsigned len);
			void synrcvd(TCP_Segment* segment, unsigned len);
			void synsent(TCP_Segment* segment, unsigned len);
			void finwait1(TCP_Segment* segment, unsigned len);
			void finwait2(TCP_Segment* segment, unsigned len);
			void listen(TCP_Segment* segment, unsigned len);
			void lastack(TCP_Segment* segment, unsigned len);
			void established(TCP_Segment* segment, unsigned len);

			// **************************************************************************
			// *** TCP Receiving ***
			// **************************************************************************
			bool receiving; // is set true if application called receive(...)
			unsigned application_recv_len; //actual length that has been read

			TCP_ReceiveBuffer receiveBuffer;
			friend class TCP_ReceiveBuffer; //allow TCP_ReceiveBuffer to use our freeReceivedSegment(...) function

			unsigned maxReceiveWindow_MSS; //in units of mss
			unsigned maxReceiveWindow_Bytes; //in bytes (ie. maxReceiveWindow_MSS * mss)
			unsigned mss; // maximum segment size
			/**
			 * Set the maximum number of bytes for receiving/sending per tcp segment.
			 * Congestion control may reduce this value.
			 * Usually you would take the path MTU and subtract the IP-header size.
			 */
			void setMSS(unsigned max_segment_size);

			bool handleData(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len);

			uint16_t getReceiveWindow();

            void recv_loop();

			/**
			* This is a hack: Because the state machine works on segments instead
			* of ReceiveBuffers we need an extra free method for those segments.
			* These are always received segements from packetbuffer and therefore
			* are encapsulated into a ReceiveBuffer.
			*/
			void freeReceivedSegment(TCP_Segment* segment) ;
			
			//returns actual length that has been read
            inline unsigned get_application_recv_len() { return application_recv_len; }

			//return *all* bytes remaining in the receive buffer (for polling)
            inline unsigned getRecvBytes() { return receiveBuffer.getRecvBytes(); }

			/**
			 * Bind this socket to the source port you have set before.
			 */
			bool bind();
			
			/**
			 * Unbind this socket from the source port you have set before.
			 * This is called automatically on destruction of the socket. 
			 */
			void unbind();
			
			// **************************************************************************
			// *** TCP Sending ***
			// **************************************************************************
			uint32_t seqnum_unacked;
			uint32_t seqnum_next;
            void set_seqnum_unacked(uint32_t acknum) { seqnum_unacked = acknum; }

			// Cannot be inlined! Overwritten by "RTT estimation"
			uint32_t getRTO() { return DEFAULT_RTO; }

            uint16_t sendWindow; //advertised window of remote peer
            uint32_t lwack, lwseq; //Acknum and Seqnum of last window update (sendWindow)
            void initSendWindow(uint16_t window, uint32_t seqnum, uint32_t acknum);
            void updateSendWindow(TCP_Segment* segment, uint32_t seqnum, uint32_t acknum);
            uint16_t getSendWindow() { return sendWindow; }
            inline void lowerSendWindow(uint16_t subtract) { sendWindow -= subtract; }

            TCP_History history;
			void clearHistory();
            void updateHistory(bool do_retransmit = true);
            void retransmit(ipstack::TCP_Record* record);

			void processSendData();
			bool sendNextSegment();
			bool sendSegment(uint_fast16_t len);
			void writeHeader(SendBuffer* sendbuffer);
			void writeHeaderWithAck(SendBuffer* sendbuffer, uint32_t ack);

			SendBuffer* requestSendBufferTCP(uint_fast16_t payloadsize = 0) ;
			/**
			* Returns a ready to send tcp SYN packet.
			* This is an extra function because SYN packets may be handled in special
			* ways in tcp. E.g. the maximum segment size option is only valid in SYN
			* packets.
			*/
			SendBuffer* requestSendBufferTCP_syn(uint_fast16_t payloadsize = 0);

			// **************************************************************************
			// *** TCP Packet Flags Handler ***
			// **************************************************************************
			bool FIN_received;
			uint32_t FIN_seqnum;

			void handleFIN(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len) {
				if (segment->has_FIN() && (FIN_received == false)) {
					FIN_seqnum = seqnum + payload_len;
					FIN_received = true;
				}
			}

			bool FIN_complete();

			bool handleSYN(TCP_Segment* segment);

			bool handleSYN_final(TCP_Segment* segment);

			bool handleRST(TCP_Segment* segment);


			// **************************************************************************
			// *** TCP Acknowledgements ***
			// **************************************************************************
			bool ACK_triggered;

			void triggerACK() {
				ACK_triggered = true;
			}

			bool sendACK(uint32_t ackNum);
			bool sendACK() {
				ACK_triggered = false;
				return sendACK(receiveBuffer.getAckNum());
			}

			void handleACK(uint32_t acknum);
			void processACK();


			// **************************************************************************
			// *** Common ***
			// **************************************************************************

			uint16_t dport;
			uint16_t sport;

			void* application_buf; //pointer to application read/write buffer
			unsigned application_buflen; //length of above to be read/written

			/** 
			* To be influenced by header expanding options.
			* Is used for the header-size field in the tcp header and for reserving fitting
			* memory blocks.
			*/
			uint8_t getSpecificTCPHeaderSize(){ return TCP_Segment::TCP_MIN_HEADER_SIZE; }
			void gen_initial_seqnum();

			//wait for incoming packet for a given time (timeout)
			//false := packet arrived, true := timeout reached
			bool block(uint32_t timeout);

			void resetSocketState();
			void abort();
	};
	
	/**
	* Streaming operator for the TCP_Socket.
	* Usage example:
	* TCP_Socket t;
	* t << "some data" << "another data" << 11;
	*/
	class TCP_Socket;
	template<class T> inline TCP_Socket &operator <<(TCP_Socket &obj, T arg) { obj.write(arg,sizeof(arg)); return obj; }
	template<> inline TCP_Socket &operator <<(TCP_Socket &obj, const char* arg) { obj.write(arg,strlen(arg)); return obj; }

} //namespace ipstack

