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


#include "TCP_Socket.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	TCP_Socket::TCP_Socket(const SocketMemory& memory) : SocketMemory(memory),
		receiveBuffer(this),
		dport(TCP_Segment::UNUSED_PORT),
		sport(TCP_Segment::UNUSED_PORT)
	{
		setMaxReceiveWindow(get_Mempool()->getMaxFreeBlockSize());
		resetSocketState();
	}

	TCP_Socket::~TCP_Socket() {
		unbind();
	}

	bool TCP_Socket::listen()
	{
		if (isClosed()) {
			gen_initial_seqnum();
			sendWindow = 0;
			state = LISTEN; // next state
			waiting = true;
			return true;
		}
		return false;
	}

	bool TCP_Socket::connect()
	{
		if (!hasValidSrcDestAddresses()) {
			return false; //no (valid) dst_ipv4_addr set
		}
		if (isClosed()) {
			//only allow connecting in CLOSED state
			set_sport(TCP_Segment::UNUSED_PORT); //reset source port
			if (bind()) {
				if (isClosed()) {
					gen_initial_seqnum();
					sendWindow = 0;

					SendBuffer* b = requestSendBufferTCP_syn();
					if (b != 0) {
						TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
						writeHeader(b);
						segment->set_SYN();
						seqnum_next++; // 1 seqnum consumed
						state = SYNSENT; // next state
						send(b);
						waiting = true;
						history.add(b, getRTO());
					}
				}
				recv_loop();
				return isEstablished();
			}
		}
		return false;
	}

	void TCP_Socket::abort()
	{
		TCP_Socket_Private::abort();
	}

	//full close (no half-close supported)
	bool TCP_Socket::close()
	{
		if (isCloseWait() || isEstablished()) {
			//send FIN:
			SendBuffer* b = requestSendBufferTCP();
			if (b != 0) {
				TCP_Segment* segment = (TCP_Segment*)b->getDataPointer();
				writeHeaderWithAck(b, receiveBuffer.getAckNum());
				segment->set_FIN();
				seqnum_next++; // 1 seqnum consumed
				send(b);
				waiting = true;
				history.add(b, getRTO());
				if (isCloseWait()) {
					state = LASTACK; //passive close
				} else { //if(state == ESTABLISHED)
					state = FINWAIT1; //active close
				}
			}
		}
		//else{
		//  printf("ERROR: close() must be in ESTABLISHED or CLOSEWAIT\n");
		//}
		recv_loop();
		unbind();
		return isClosed();
	}

	void TCP_Socket::set_dport(uint16_t d)
	{
		dport = d;
	}

	uint16_t TCP_Socket::get_dport()
	{
		return dport;
	}

	void TCP_Socket::set_sport(uint16_t s)
	{
		sport = s;
	}

	uint16_t TCP_Socket::get_sport()
	{
		return sport;
	}

	int TCP_Socket::poll(unsigned int msec)
	{
		if (isEstablished() || isCloseWait()) {
			ReceiveBuffer* receiveB = (ReceiveBuffer*)packetbuffer->get();
			if (receiveB == 0) {
				//no gratuitous packets received, yet
				bool timeout_reached = (msec==0) ? false : block((uint32_t)msec); //wait for max. msec
				if (timeout_reached == false) {
					receiveB = (ReceiveBuffer*)packetbuffer->get(); //check for new packet, since timeout was not reached
				}
			}
			if (receiveB != 0) {
				do {
					//process all gratuitous packets that have arrived
					input((TCP_Segment*) receiveB->getData(), receiveB->getSize());
					receiveB = (ReceiveBuffer*)packetbuffer->get();
				} while (receiveB != 0);
				updateHistory(); //cleanup packets which are not used anymore
				processACK(); //send ACK if necessary
			}

			//return amount of received data bytes (payload) while polling
			//zero out MSB, because return value is an int!
			const unsigned msb = 1 << ((8 * sizeof(unsigned)) - 1);
			return (int)(getRecvBytes() & ~msb);
		}
		return -1;
	}

	int TCP_Socket::receive(void* buffer, unsigned int buffer_len)
	{
		if (isEstablished() || isCloseWait()) {
			application_buf = buffer;
			application_buflen = buffer_len;
			application_recv_len = 0;
			receiving = true;
			waiting = true;
			recv_loop();
			unsigned recv_len = get_application_recv_len();
			if ((recv_len == 0) && isCloseWait()) {
				return -2;
			}
			const unsigned msb = 1 << ((8 * sizeof(unsigned)) - 1);
			return (int)(recv_len & ~msb);
		}
		return -1;
	}

	bool TCP_Socket::write(const void* data, unsigned int datasize)
	{
		if (isEstablished() || isCloseWait()) {
			application_buf = (void*) data;
			application_buflen = datasize;
			waiting = true;
			recv_loop();
			return (isEstablished() || isCloseWait()); //we are still is a state that allows sending
		}
		return false;
	}

} //namespace ipstack
