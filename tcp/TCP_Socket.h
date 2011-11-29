#ifndef __TCP_SOCKET__
#define __TCP_SOCKET__

#include "../util/Mempool.h"
#include "TCP.h"
#include "tcp_history/TCP_History.h"
#include "tcp_receivebuffer/TCP_ReceiveBuffer.h"

#include "../Clock.h"

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
         
  protected:
  bool isEstablished(){ return state == ESTABLISHED; }
  bool isCloseWait(){ return state == CLOSEWAIT; }
  bool isListening(){ return state == LISTEN; }
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
  friend class TCP_ReceiveBuffer; //allow TCP_ReceiveBuffer to use our free(...) function
  
  unsigned maxReceiveWindow_MSS; //in units of mss
  unsigned maxReceiveWindow_Bytes; //in bytes (ie. maxReceiveWindow_MSS * mss)
  
  bool handleData(TCP_Segment* segment, UInt32 seqnum, unsigned payload_len);
  
  UInt16 getReceiveWindow();
  
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

  //return *all* bytes remaining in the receive buffer (for polling)
  unsigned getRecvBytes(){
    return receiveBuffer.getRecvBytes();
  }
  
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
  
  enum { DEFAULT_RTO = 6000U }; //the default (6 sec) 'retransmission timeout'
  UInt32 getRTO() { return DEFAULT_RTO; } //overwritten by "RTT estimation'
  
  void set_seqnum_unacked(UInt32 acknum){ seqnum_unacked = acknum; }
  
  void initSendWindow(UInt16 window, UInt32 seqnum, UInt32 acknum){
    sendWindow = window;
    lwseq = seqnum;
    lwack = acknum;
  }
  
  UInt16 getSendWindow() { return sendWindow; }
  void lowerSendWindow(UInt16 subtract){ sendWindow -= subtract; }
  
  unsigned min(unsigned a, unsigned b, unsigned c){
    return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
  }
  
  void updateSendWindow(TCP_Segment* segment, UInt32 seqnum, UInt32 acknum);

  void retransmit(TCP_Record* record, TCP_Segment* segment){
    send(segment, record->getLength());
    record->setTimeout(getRTO());
  }
  
  void clearHistory();
  
  void processSendData();
  bool sendNextSegment();
  
  protected:
  void updateHistory(bool do_retransmit=true);

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
      updateHistory(false); //update history ... (false := do not trigger any retransmissions)
      if(history.isFull()){ // ... and try again
        return 0;
      }
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
  
  bool FIN_complete();

  bool handleSYN(TCP_Segment* segment);

  bool handleSYN_final(TCP_Segment* segment);
    
  bool handleRST(TCP_Segment* segment);
  
  
  // **************************************************************************
  // *** TCP Acknowledgements ***
  // **************************************************************************
  private:
  bool ACK_triggered;
  
  void triggerACK(){ ACK_triggered = true; }
  
  bool sendACK(UInt32 ackNum);
  bool sendACK() { ACK_triggered = false; return sendACK(receiveBuffer.getAckNum()); }
  
  void handleACK(UInt32 acknum);

  protected:
  void processACK();
  
  
  // **************************************************************************
  // *** Common ***
  // **************************************************************************
  private:
  TCP_Socket(const TCP_Socket &copy); //prevent copying
  
  UInt16 dport;
  UInt16 sport;
  
  unsigned mss;
  
  void* application_buf; //pointer to application read/write buffer
  unsigned application_buflen; //length of above to be read/written
  
  bool waiting;
  
  void setupHeader(TCP_Segment* segment);
  
  void setupHeader_withACK(TCP_Segment* segment);

  void gen_initial_seqnum(){
    //RFC 793 (TCP Illustrated Vol.1 page 232)
    UInt64 usec = Clock::ticks_to_ms(Clock::now()) * 1000UL; // microseconds
    seqnum_next = usec/4; // 'increment' every 4 usec ;-)
    seqnum_unacked = seqnum_next;
  }
  
  void abort();
  
  protected:
  bool waiting_for_input() { return waiting; }

  //wait for incoming packet for a given time (timeout)
  //false := packet arrived, true := timeout reached
  bool block(UInt32 timeout);
  void block(); //wait for incoming packets only

  void setMSS(unsigned max_segment_size);
  
  void setMTU(unsigned mtu){
    setMSS(TCP_Segment::DEFAULT_MSS);
    //mss is set to mtu by external aspect: 'tcp_options/MSS.ah'
  }
  
  void close();
  
  void connect();
 
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
