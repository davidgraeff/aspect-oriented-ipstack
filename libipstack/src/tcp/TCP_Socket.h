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

#include "util/Mempool.h"
#include "util/Ringbuffer.h"
#include "demux/DemuxLinkedListContainer.h"
#include "TCP.h"
#include "tcp_history/TCP_History.h"
#include "tcp_receivebuffer/TCP_ReceiveBuffer.h"

#include <string.h>

namespace ipstack
{
	class SendBuffer;
class TCP_Socket : public DemuxLinkedList<TCP_Socket>
{

		// **************************************************************************
		// *** TCP Statemachine ***
		// **************************************************************************
	private:
		enum { CLOSED, LISTEN, SYNSENT, SYNRCVD, ESTABLISHED, FINWAIT1, FINWAIT2, CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT } state;
	protected:
		bool isEstablished() { return state == ESTABLISHED; }
		bool isCloseWait() { return state == CLOSEWAIT; }
		bool isListening() { return state == LISTEN; }
		bool isClosed() { return state == CLOSED; }
		bool isSynRcvd() { return state == SYNRCVD; }

		// *THE* state machine switch
		void input(TCP_Segment* segment, unsigned len);


		// **************************************************************************
		// *** TCP Receiving ***
		// **************************************************************************
	private:
		bool receiving; // is set true if application called receive(...)

		unsigned application_recv_len; //actual length that has been read

		TCP_ReceiveBuffer receiveBuffer;
		friend class TCP_ReceiveBuffer; //allow TCP_ReceiveBuffer to use our freeReceivedSegment(...) function

		unsigned maxReceiveWindow_MSS; //in units of mss
		unsigned maxReceiveWindow_Bytes; //in bytes (ie. maxReceiveWindow_MSS * mss)

		bool handleData(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len);

		uint16_t getReceiveWindow();

	protected:
		bool packetBufferFull() {
			return packetbuffer->isFull();
		}
		
		/**
		 * Add a tcp segment to this socket (usually from the ip layer below)
		 * Return true if the necessary buffer could be reserved.
		 */
		bool addToReceiveQueue (TCP_Segment* segment, unsigned segment_len) ;
		
		/**
		 * This is a hack: Because the state machine works on segments instead
		 * of ReceiveBuffers we need an extra free method for those segments.
		 * These are always received segements from packetbuffer and therefore
		 * are encapsulated into a ReceiveBuffer.
		 */
		void freeReceivedSegment(TCP_Segment* segment) ;
		
		//returns actual length that has been read
		unsigned get_application_recv_len() {
			return application_recv_len;
		}

		//return *all* bytes remaining in the receive buffer (for polling)
		unsigned getRecvBytes() {
			return receiveBuffer.getRecvBytes();
		}

	public:
		void setMaxReceiveWindow(unsigned max) {
			maxReceiveWindow_MSS = max;
		}


		// **************************************************************************
		// *** TCP Sending ***
		// **************************************************************************
	private:
		uint32_t seqnum_unacked;
		uint32_t seqnum_next;

		TCP_History history;

		uint16_t sendWindow; //advertised window of remote peer
		uint32_t lwack, lwseq; //Acknum and Seqnum of last window update (sendWindow)

		enum { DEFAULT_RTO = 6000U }; //the default (6 sec) 'retransmission timeout'
		uint32_t getRTO() {
			return DEFAULT_RTO;    //overwritten by "RTT estimation'
		}

		void set_seqnum_unacked(uint32_t acknum) {
			seqnum_unacked = acknum;
		}

		void initSendWindow(uint16_t window, uint32_t seqnum, uint32_t acknum) {
			sendWindow = window;
			lwseq = seqnum;
			lwack = acknum;
		}

		uint16_t getSendWindow() {
			return sendWindow;
		}
		void lowerSendWindow(uint16_t subtract) {
			sendWindow -= subtract;
		}

		unsigned min(unsigned a, unsigned b, unsigned c) {
			return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
		}

		void updateSendWindow(TCP_Segment* segment, uint32_t seqnum, uint32_t acknum);

		void retransmit(ipstack::TCP_Record* record);

		void clearHistory();

		void processSendData();
		bool sendNextSegment();
		bool sendSegment(uint_fast16_t len);

	protected:
		void updateHistory(bool do_retransmit = true);



		// **************************************************************************
		// *** Memory Management (Mempool) ***
		// **************************************************************************
	private:
		Mempool* mempool;
		Packetbuffer* packetbuffer;
		Packetbuffer* get_packetbuffer() { return packetbuffer; }
		
		SendBuffer* requestSendBufferTCP(uint_fast16_t payloadsize = 0) ;
		/**
		 * Returns a ready to send tcp SYN packet.
		 * This is an extra function because SYN packets may be handled in special
		 * ways in tcp. E.g. the maximum segment size option is only valid in SYN
		 * packets.
		 */
		SendBuffer* requestSendBufferTCP_syn(uint_fast16_t payloadsize = 0);
		
	protected:
		void set_Mempool(Mempool* m) { mempool = m; }
		void set_packetbuffer(Packetbuffer* pb) { packetbuffer = pb; }
		
	public: Mempool* get_Mempool() { return mempool; }

		// **************************************************************************
		// *** TCP Packet Flags Handler ***
		// **************************************************************************
	private:
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
	private:
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

	protected:
		void processACK();


		// **************************************************************************
		// *** Common ***
		// **************************************************************************
	private:
		TCP_Socket(const TCP_Socket& copy); //prevent copying

		uint16_t dport;
		uint16_t sport;

		unsigned mss;

		void* application_buf; //pointer to application read/write buffer
		unsigned application_buflen; //length of above to be read/written

		bool waiting;

		void writeHeader(SendBuffer* sendbuffer);
		void writeHeaderWithAck(SendBuffer* sendbuffer, uint32_t ack);

		/** 
		 * To be influenced by header expanding options.
		 * Is used for the header-size field in the tcp header and for reserving fitting
		 * memory blocks.
		 */
		uint8_t getSpecificTCPHeaderSize(){return TCP_Segment::TCP_MIN_HEADER_SIZE;}
		void gen_initial_seqnum();
		void abort();

		void recv_loop();
		
	protected:
		bool waiting_for_input() {
			return waiting;
		}

		//wait for incoming packet for a given time (timeout)
		//false := packet arrived, true := timeout reached
		bool block(uint32_t timeout);
		void block(); //wait for incoming packets only

		void setMSS(unsigned max_segment_size);

		bool connect();


		//contructor:
		TCP_Socket();
		~TCP_Socket();
		void resetSocketState();

	public:
		bool bind() ;
		void unbind() ;
		bool listen();
		
		/**
		 * Use this method to write a lot of data as stream over this tcp connection.
		 */
		bool write(const void* data, unsigned datasize);
		
		/**
		 * Use this api to receive a lot of data as stream over this tcp connection
		 */
		int receive(void* buffer, unsigned buffer_len);
		
		/**
		 * wait for msec for a new packet to arive
		 */
		int poll(unsigned msec);
		
		/**
		 * close a tcp connection
		 */
		bool close();
		void set_dport(uint16_t d) ;
		uint16_t get_dport() ;
		void set_sport(uint16_t s) ;
		uint16_t get_sport() ;
};

} //namespace ipstack

