#ifndef __TCP_SOCKET__
#define __TCP_SOCKET__

#include "../util/Mempool.h"
#include "TCP.h"
#include "tcp_history/TCP_History.h"
#include "tcp_receivebuffer/TCP_ReceiveBuffer.h"

#include "hw/hal/SystemClock.h"

#include <string.h>

namespace ipstack {
  

class TCP_Socket{
  
  // **************************************************************************
  // *** TCP Statemachine ***
  // **************************************************************************
  private:
  enum { CLOSED, LISTEN, SYNSENT, SYNRCVD,
         ESTABLISHED, FINWAIT1, FINWAIT2,
         CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT
       } state;
         
  void closed(TCP_Segment* segment, unsigned len);
  void listen(TCP_Segment* segment, unsigned len);
  void synsent(TCP_Segment* segment, unsigned len);
  void synrcvd(TCP_Segment* segment, unsigned len);
  void established(TCP_Segment* segment, unsigned len);
  void finwait1(TCP_Segment* segment, unsigned len);
  void finwait2(TCP_Segment* segment, unsigned len);
  void closewait(TCP_Segment* segment, unsigned len);
  void lastack(TCP_Segment* segment, unsigned len);
  void closing(TCP_Segment* segment, unsigned len);
  void timewait(TCP_Segment* segment, unsigned len);
  
  protected:
  bool isEstablished(){ return state == ESTABLISHED; }
  bool isCloseWait(){ return state == CLOSEWAIT; }
  bool isListening(){ return state == LISTEN; }
  bool isClosed() { return state == CLOSED; }
  bool isSynRcvd() { return state == SYNRCVD; }
  
  // *THE* state machine switch
  void input(TCP_Segment* segment, unsigned len){
    switch(state){
      case CLOSED: closed(segment, len); break;
      case LISTEN: listen(segment, len); break;
      case SYNSENT: synsent(segment, len); break;
      case SYNRCVD: synrcvd(segment, len); break;
      case ESTABLISHED: established(segment, len); break;
      case FINWAIT1: finwait1(segment, len); break;
      case FINWAIT2: finwait2(segment, len); break;
      case CLOSEWAIT: closewait(segment, len); break;
      case LASTACK: lastack(segment, len); break;
      case CLOSING: closing(segment, len); break;
      case TIMEWAIT: timewait(segment, len); break;
      //default: CLOSED ?
    }
  }
  
  
  // **************************************************************************
  // *** TCP Receiving ***
  // **************************************************************************
  private:
  bool receiving; // is set true if application called receive(...)
  
  unsigned application_recv_len; //actual length that has been read
  
  TCP_ReceiveBuffer receiveBuffer;
  friend class TCP_ReceiveBuffer; //allow TCP_ReceiveBuffer to use our free(...) function
  
  unsigned maxReceiveWindow_MSS; //in units of mss
  unsigned maxReceiveWindow_Bytes; //in bytes (ie. maxReceiveWindow_MSS * mss)
  
  bool handleData(TCP_Segment* segment, UInt32 seqnum, unsigned payload_len);
  
  UInt16 getReceiveWindow(){
    if(state == SYNSENT || state == CLOSED){
      return (UInt16)TCP_Segment::DEFAULT_MSS; //no mss negotiated so far
    }
    else{
      unsigned currRecvWnd = maxReceiveWindow_Bytes - receiveBuffer.getRecvBytes();
      if(currRecvWnd > 0xFFFFU){
        //advertised window is only 16 bit wide
        return 0xFFFFU;
      }
      else{
        return (UInt16)currRecvWnd;
      }
    }
  }
  
  protected:
  void receive(void* buffer, unsigned buffer_len){
    application_buf = buffer;
    application_buflen = buffer_len;
    application_recv_len = 0;
    receiving = true;
    waiting = true;
  }
  
  //returns actual length that has been read
  unsigned get_application_recv_len() { return application_recv_len; }
  
  public:
  void setMaxReceiveWindow(unsigned max) { maxReceiveWindow_MSS = max; }
  
  
  // **************************************************************************
  // *** TCP Sending ***
  // **************************************************************************
  private:
  UInt32 seqnum_unacked;
  UInt32 seqnum_next;
  
  TCP_History history;
  
  UInt16 sendWindow; //advertised window of remote peer
  UInt32 lwack, lwseq; //Acknum and Seqnum of last window update (sendWindow)
  
  enum { DEFAULT_RTO = 5000U }; //the default (5 sec) 'retransmission timeout'
  UInt32 getRTO() { return DEFAULT_RTO; } //overwritten by "RTT estimation'
  
  void set_seqnum_unacked(UInt32 acknum){ seqnum_unacked = acknum; }
  
  void initSendWindow(TCP_Segment* segment, UInt32 seqnum, UInt32 acknum){
    sendWindow = segment->get_window();
    lwseq = seqnum;
    lwack = acknum;
  }
  
  UInt16 getSendWindow() { return sendWindow; }
  void lowerSendWindow(UInt16 subtract){ sendWindow -= subtract; }
  
  unsigned min(unsigned a, unsigned b, unsigned c){
    return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
  }
  
  void updateSendWindow(TCP_Segment* segment, UInt32 seqnum, UInt32 acknum){
    if(TCP_Segment::SEQ_LT(seqnum, lwseq)){
      return; // this segment arrived out-of-order
      //we aleary have a newer window update
    }
    if( (seqnum == lwseq) &&
       TCP_Segment::SEQ_LT(acknum, lwack)) {
      //this acknowledgment arrived out-of-order
      //we already have received a more recent acknowledgment
      return;
    }
    //else, we have a send window update:
    
    sendWindow = segment->get_window();
    lwseq = seqnum;
    lwack = acknum;
  }

  void updateHistory();
  
  bool retransmit(TCP_Record* record, TCP_Segment* segment){
    if(segment->get_seqnum() == seqnum_unacked){
      //this is the first element that got lost.
      //only retransmit this one
      resend(record, segment);
      return true;
    }
    return false;
  }
  
  void resend(TCP_Record* record, TCP_Segment* segment){
    send(segment, record->getLength());
    record->setTimeout(getRTO());
  }
  
  void clearHistory(){
    while(TCP_Record* record = history.get()){
      free(record->getSegment());
      history.remove(record);
    }
  }
  
  void processSendData();
  bool sendNextSegment();
  
  protected:
  void addSendData(const void* data, unsigned datasize){
    application_buf = (void*) data;
    application_buflen = datasize;
    waiting = true;
  }
  
  
  // **************************************************************************
  // *** Memory Management (Mempool) ***
  // **************************************************************************
  private:
  Mempool* mempool;
  unsigned network_header_offset; //Offset for IP-Header ( + Ethernet header)
  
  void* alloc(unsigned datasize){
    if(history.isFull()){
      //updateHistory(); //update history ...
      //if(history.isFull()){ // ... and try again
        return 0;
      //}
    }
    void* buffer = mempool->alloc(datasize + network_header_offset);
    if(buffer != 0){
      return (((UInt8*)buffer) + network_header_offset);
    }
    else{
      return 0;
    }
  }
  
  protected:
  inline void free(void* packet){
    // _ALWAYS_ use this method and _NEVER_ use mempool->free() directly,
    // because only this one is interrupt-safe (IRQ_Sync.ah)!
    mempool->free(packet);
  }
  
  void set_network_header_offset(unsigned offset) { network_header_offset = offset; }
  
  public:
  void set_Mempool(Mempool* m) { mempool = m; }
  Mempool* get_Mempool() { return mempool; }
  
  
  // **************************************************************************
  // *** Delegation to Network Socket (IPv4, ...) ***
  // **************************************************************************
  private:
  void* network_socket; //this pointer for IPv*_Socket
  void (*send_ptr)(void*, void*, unsigned, UInt8); //function pointer to send
  bool (*hasBeenSent_ptr)(void*, const void*); //function pointer to hasBeenSent
  
  inline void send(void* data, unsigned datasize){
    send_ptr(network_socket, data, datasize, TCP_Segment::IPV4_TYPE_TCP);
  }
  
  inline bool hasBeenSent(const void* data){
    return hasBeenSent_ptr(network_socket, data);
  }
  
  
  // **************************************************************************
  // *** TCP Packet Flags Handler ***
  // **************************************************************************
  private:
  bool FIN_received;
  UInt32 FIN_seqnum;
  
  void handleFIN(TCP_Segment* segment, UInt32 seqnum, unsigned payload_len){
    if(segment->has_FIN() && (FIN_received == false)){
      FIN_seqnum = seqnum + payload_len;
      FIN_received = true;
    }
  }
  
  bool FIN_complete(){
    //return true if a FIN was received and all data was received completely, too
    if((FIN_received == true) && (receiveBuffer.getAckNum() == FIN_seqnum)){
      sendACK(FIN_seqnum + 1U); // a FIN consumes one sequence number
      return true;
    }
    return false;
  }

  bool handleSYN(TCP_Segment* segment){
    //This function is called only at: ESTABLISHED
    if(segment->has_SYN()){
      // 1) ACK of 'three way handshake' got lost! // && segment->has_ACK()){
      // 2) OR: //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
      free(segment);
      sendACK(); // 'retransmit'
      return true;
    }
    return false;
  }

  bool handleSYN_final(TCP_Segment* segment){
    //call this function if a FIN was received
    //This function is called at: CLOSEWAIT, LASTACK, CLOSING, TIMEWAIT
    if(segment->has_SYN()){
      //rfc793 page 34 (Figure 10.) ("Half-Open Connection Discovery")
      //TODO: "Internetworking with TCP/IP II page 202": answer a SYN with RST and abort()
      free(segment);
      sendACK(FIN_seqnum + 1U);
      return true;
    }
    return false;
  }
    
  bool handleRST(TCP_Segment* segment){
    if(segment->has_RST()){
      /*In all states except SYN-SENT, all reset (RST) segments are validated
      by checking their SEQ-fields.  A reset is valid if its sequence number
      is in the window. -> TODO */
      free(segment);
      abort(); //abort the connection
      return true; 
    }
    return false;
  }
  
  void synsent_handle_other(TCP_Segment* segment, unsigned len);
  
  
  // **************************************************************************
  // *** TCP Acknowledgements ***
  // **************************************************************************
  private:
  bool ACK_triggered;
  
  void triggerACK(){ ACK_triggered = true; }
  
  void processACK() {
    if(ACK_triggered == true){
      sendACK();
    }
  }
  
  bool sendACK(UInt32 ackNum);
  bool sendACK() { ACK_triggered = false; return sendACK(receiveBuffer.getAckNum()); }
  
  void handleACK(TCP_Segment* segment, UInt32 acknum);
  
  
  // **************************************************************************
  // *** Common ***
  // **************************************************************************
  private:
  UInt16 dport;
  UInt16 sport;
  
  unsigned mss;
  
  void* application_buf; //pointer to application read/write buffer
  unsigned application_buflen; //length of above to be read/written
  
  bool waiting;
  
  void setupHeader(TCP_Segment* segment){
    segment->set_dport(dport);
    segment->set_sport(sport);
    segment->set_checksum(0);
    segment->set_seqnum(seqnum_next);
    segment->set_acknum(0);
    segment->set_header_len(TCP_Segment::TCP_MIN_HEADER_SIZE/4);
    segment->set_flags(0);
    segment->set_window(getReceiveWindow());
    segment->set_urgend_ptr(0);
  }
  
  void setupHeader_withACK(TCP_Segment* segment){
    setupHeader(segment);
    segment->set_ACK();
    segment->set_acknum(receiveBuffer.getAckNum());
  }
  
  void gen_initial_seqnum(){
    //RFC 793 (TCP Illustrated Vol.1 page 232)
    hw::hal::SystemClock& clock = hw::hal::SystemClock::Inst();
    UInt64 usec = (clock.value() / (clock.freq() / 1000000)); // microseconds
    seqnum_next = usec/4; // 'increment' every 4 usec ;-)
    seqnum_unacked = seqnum_next;
  }
  
  //wait for incoming packet for a given time (timeout)
  //false := packet arrived, true := timeout reached
  bool block(UInt32 timeout) { return false; }  
  void block() {} //wait for incoming packets only
  
  void abort();
  
  protected:
  bool waiting_for_input() { return waiting; }
  
  void setMSS(unsigned max_segment_size) {
    mss = max_segment_size;
    
    if(sizeof(unsigned) == 2){
      //8 or 16 bit machine
      UInt32 maxRecvWnd = ((UInt32)maxReceiveWindow_MSS) * max_segment_size;
      if(maxRecvWnd > 0xFFFFU){
        //limit to 16 bit (65 KByte)
        maxReceiveWindow_Bytes = 0xFFFFU;
      }
      else{
        maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
      }
    }
    else{
      // 32 or 64 bit machine
      UInt64 maxRecvWnd = ((UInt64)maxReceiveWindow_MSS) * max_segment_size;
      if(maxRecvWnd > 0xFFFFFFFFUL){
        //limit to 32 bit (4 GByte)
        maxReceiveWindow_Bytes = 0xFFFFFFFFUL;
      }
      else{
        maxReceiveWindow_Bytes = (unsigned) maxRecvWnd;
      }
    }
  }
  
  void setMTU(unsigned mtu){
    setMSS(TCP_Segment::DEFAULT_MSS);
    //mss is set to mtu by external aspect: 'tcp_options/MSS.ah'
  }
  
  void close();
  
  void connect(){
    if(state == CLOSED){
      gen_initial_seqnum();
      sendWindow = 0;
      TCP_Segment* packet = (TCP_Segment*) alloc(TCP_Segment::TCP_MIN_HEADER_SIZE);
      if(packet != 0){
        setupHeader(packet);
        seqnum_next++; // 1 seqnum consumed
        packet->set_SYN();
        state = SYNSENT; // next state
        send(packet, TCP_Segment::TCP_MIN_HEADER_SIZE);
        waiting = true;
        history.add(packet, TCP_Segment::TCP_MIN_HEADER_SIZE, getRTO());
      }
    }
  }
 
  void listen(){
    if(state == CLOSED){
      gen_initial_seqnum();
      sendWindow = 0;
      state = LISTEN; // next state
      waiting = true;
    }
  }
  
  //contructor:
  TCP_Socket(void* socket, void (*send)(void *, void*, unsigned, UInt8), bool (*hasBeenSent)(void*, const void*)) :
    state(CLOSED),
    receiving(false),
    receiveBuffer(this),
    maxReceiveWindow_MSS(1),
    maxReceiveWindow_Bytes(TCP_Segment::DEFAULT_MSS),
    network_header_offset(0),
    network_socket(socket),
    send_ptr(send),
    hasBeenSent_ptr(hasBeenSent),
    FIN_received(false),
    ACK_triggered(false),
    dport(TCP_Segment::UNUSED_PORT),
    sport(TCP_Segment::UNUSED_PORT),
    mss(TCP_Segment::DEFAULT_MSS),
    application_buflen(0),
    waiting(false) {}

  public:
  void set_dport(UInt16 d) { dport = d; }
  UInt16 get_dport() { return dport; }
  
  void set_sport(UInt16 s) { sport = s; }
  UInt16 get_sport() { return sport; }
  
  void setMaxMTU(unsigned max_mtu) {} //affected by 'tcp_options/MSS.ah'
};

} //namespace ipstack

#endif // __TCP_SOCKET__
