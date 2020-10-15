#include "GoBackNRdtReceiver.h"
#include "Global.h"
GoBackNRdtReceiver::GoBackNRdtReceiver() {
    lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;	//忽略该字段
    for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
        lastAckPkt.payload[i] = '.';
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

GoBackNRdtReceiver::~GoBackNRdtReceiver() {

}

void GoBackNRdtReceiver::receive(const Packet &packet) {
    if(packet.seqnum == lastAckPkt.acknum + 1){
        if(pUtils->calculateCheckSum(packet) == packet.checksum){
            lastAckPkt.acknum ++;
            pUtils->printPacket("[GBN RECEIVER] packet received, delivering to app layer", packet);
            Message msg;
            memcpy(msg.data, packet.payload, sizeof(packet.payload));
            pns->delivertoAppLayer(RECEIVER, msg);
            return;
        }
        else {
            pUtils->printPacket("[GBN RECEIVER] packet broken", packet);
        }
    }
    else {
        pUtils->printPacket("[GBN RECEIVER] packet sequence number error", packet);
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
    pns->sendToNetworkLayer(SENDER, lastAckPkt);
}
