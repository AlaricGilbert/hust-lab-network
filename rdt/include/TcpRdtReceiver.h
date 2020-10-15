#ifndef STOP_WAIT_TCP_RDT_RECEIVER_H
#define STOP_WAIT_TCP_RDT_RECEIVER_H
#include "RdtReceiver.h"
class TcpRdtReceiver: public RdtReceiver
{
public:
    TcpRdtReceiver(int rcvWindowSz);
    ~TcpRdtReceiver();
    void receive(const Packet& packet) override;

private:
    Packet ackPkt;
    int rcvWindowSize;
    int rcvBase;
    Packet* rcvBuffer;
};
#endif // !STOP_WAIT_TCP_RDT_RECEIVER_H


