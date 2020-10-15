#ifndef HUST_LAB_NETWORK_GBNRDTRECEIVER_H
#define HUST_LAB_NETWORK_GBNRDTRECEIVER_H

#include "RdtReceiver.h"
class GoBackNRdtReceiver : public RdtReceiver{
public:
    GoBackNRdtReceiver();
    ~GoBackNRdtReceiver();
    void receive(const Packet &packet) override;

private:
    Packet lastAckPkt;
};


#endif //HUST_LAB_NETWORK_GBNRDTRECEIVER_H
