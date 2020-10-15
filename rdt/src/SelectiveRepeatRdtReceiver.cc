#include "SelectiveRepeatRdtReceiver.h"
#include "Global.h"
SelectiveRepeatRdtReceiver::SelectiveRepeatRdtReceiver(int rcvWindowSz)
{
    rcvBuffer = new Packet[rcvWindowSz];
	rcvWindowSize = rcvWindowSz;
    rcvBase = 0;
    ackPkt.acknum = 0; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    ackPkt.checksum = 0;
    ackPkt.seqnum = -1;	//忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
        ackPkt.payload[i] = '.';
    }
    ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
}

SelectiveRepeatRdtReceiver::~SelectiveRepeatRdtReceiver()
{
    delete[] rcvBuffer;
}

void SelectiveRepeatRdtReceiver::receive(const Packet& packet)
{
    if (pUtils->calculateCheckSum(packet) == packet.checksum) {
        pUtils->printPacket("[SR RECEIVER] packet received, receive window may be moved", packet);

        rcvBuffer[packet.seqnum % rcvWindowSize] = packet;
        if (packet.seqnum == rcvBase) {
            Message msg;
            while (rcvBuffer[rcvBase % rcvWindowSize].seqnum == rcvBase)
            {
                memcpy(msg.data, rcvBuffer[rcvBase % rcvWindowSize].payload, sizeof(rcvBuffer[rcvBase % rcvWindowSize].payload));
                pns->delivertoAppLayer(RECEIVER, msg);
                rcvBase++;
            }
            std::cout << "[SR RECEIVER] receive window moved, current receive window:" << std::endl << "[";
            for (int i = 0; i < rcvWindowSize - 1; i++)
            {
                std::cout << rcvBase + i << " ";
            }
            std::cout << rcvBase + rcvWindowSize - 1<< "]" << std::endl;
        }


        ackPkt.acknum = packet.seqnum;
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("[SR RECEIVER] sending ack packet", ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    }
}
