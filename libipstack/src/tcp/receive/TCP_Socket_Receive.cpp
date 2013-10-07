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


#include "TCP_Socket_Private.h"
#include "demux/Demux.h"
#include "router/sendbuffer/SendBuffer.h"
#include "demux/receivebuffer/ReceiveBuffer.h"
#include "os_integration/Clock.h"

namespace ipstack
{
	static uint16_t TCP_Socket::tcp_last_default_port = 1024U;

	bool TCP_Socket::bind() {
		uint16_t sport = get_sport();
		if (sport == TCP_Segment::UNUSED_PORT) {
			// Choose 'random' source port number
			// Restriction: The port is unique across all ip interfaces
			// instead unique to the current ip only.
			while (tcp_last_default_port < 65000) {
				bool foundUnusedPort = true;
				while (current != 0) {
					if (current->get_sport() == tcp_last_default_port) {
						++tcp_last_default_port;
						foundUnusedPort = false;
						break;
					}
					current = current->getNext();
				}
				if (foundUnusedPort) {
					socket->set_sport(tcp_last_default_port);
					++tcp_last_default_port; // increase for next bind()
					break;
				}
			}
			// no free port in range 1024..65000 found!
			return false;
		} else {
			//verify if connection is not used already
			TCP_Socket* current = Demux::Inst().tcp_head_socket;
			while (current != 0) {
				if (current->get_sport() == sport) {
					if (current->get_dport() == get_dport()) {
						return false;
					}
				}
			}
			current = current->getNext();
		}

		//insert at front
		setNext(tcp_head_socket);
		Demux::Inst().tcp_head_socket = this;
		return true;
	}

	void TCP_Socket::unbind() {
		if (this == Demux::Inst().tcp_head_socket) {
			Demux::Inst().tcp_head_socket = getNext();
		} else {
			TCP_Socket* current = Demux::Inst().tcp_head_socket;
			TCP_Socket* next = current->getNext();
			while (next != 0) {
				if (next == this) {
					current->setNext(next->getNext());
					return;
				}
				current = next;
				next = current->getNext();
			}
		}
	}

	uint16_t TCP_Socket::getReceiveWindow()
	{
		if (isSynSend() || isClosed()) {
			return (uint16_t)TCP_Segment::DEFAULT_MSS; //no mss negotiated so far
		} else {
			unsigned currRecvWnd = maxReceiveWindow_Bytes - receiveBuffer.getRecvBytes();
			if (currRecvWnd > 0xFFFFU) {
				//advertised window is only 16 bit wide
				return 0xFFFFU;
			} else {
				return (uint16_t)currRecvWnd;
			}
		}
	}

	void TCP_Socket::setMSS(unsigned max_segment_size)
	{
		mss = max_segment_size;

		if (sizeof(unsigned) == 2) {
			//8 or 16 bit machine
			uint32_t maxRecvWnd = ((uint32_t)maxReceiveWindow_MSS) * max_segment_size;
			if (maxRecvWnd > 0xFFFFU) {
				//limit to 16 bit (65 KByte)
				maxReceiveWindow_Bytes = 0xFFFFU;
			} else {
				maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
			}
		} else {
			// 32 or 64 bit machine
			uint64_t maxRecvWnd = ((uint64_t)maxReceiveWindow_MSS) * max_segment_size;
			if (maxRecvWnd > 0xFFFFFFFFUL) {
				//limit to 32 bit (4 GByte)
				maxReceiveWindow_Bytes = (unsigned) 0xFFFFFFFFUL;
			} else {
				maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
			}
		}
	}

	void TCP_Socket::setMaxReceiveWindow(unsigned max) {
		maxReceiveWindow_MSS = max;
	}

	void TCP_Socket::recv_loop()
	{
		while (waiting_for_input()) {
			ReceiveBuffer* receiveB = (ReceiveBuffer*)packetbuffer->get();
			if (receiveB != 0) {
				input((TCP_Segment*) receiveB->getData(), receiveB->getSize());
			} else {
				input(0, 0);
			}
		}
	}

	void TCP_Socket::freeReceivedSegment(TCP_Segment* segment)
	{
		ReceiveBuffer* receiveB = (ReceiveBuffer*)((char*)segment - sizeof(ReceiveBuffer));
		ReceiveBuffer::free(receiveB);
	}

	bool TCP_Socket::handleData(TCP_Segment* segment, uint32_t seqnum, unsigned payload_len)
	{
		bool needToFree = true; //caller must free this segment?
		if (payload_len > 0) {
			// this segment contains payload data
			needToFree = (receiveBuffer.insert(segment, seqnum, payload_len) == false);
			triggerACK();
		}
		return needToFree;
	}


} //namespace ipstack
