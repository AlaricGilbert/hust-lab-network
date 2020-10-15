#ifndef HUST_LAB_NETWORK_SRRDTRECEIVER_H
#define HUST_LAB_NETWORK_SRRDTRECEIVER_H
#include "RdtReceiver.h"
#include <map>
class SelectiveRepeatRdtReceiver : public RdtReceiver
{
public:
    SelectiveRepeatRdtReceiver(int rcvWindowSz);
    ~SelectiveRepeatRdtReceiver();
    void receive(const Packet& packet) override;

private:
    Packet ackPkt;
    int rcvWindowSize;
    int rcvBase;
    Packet* rcvBuffer;
};

#endif // HUST_LAB_NETWORK_SRRDTRECEIVER_H