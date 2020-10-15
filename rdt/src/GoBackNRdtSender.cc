#include "GoBackNRdtSender.h"
#include "Global.h"
GoBackNRdtSender::GoBackNRdtSender(int N) :N(N), base(0), nextSeqNum(0), sndBuffer(new Packet [N]), resending(false){}

GoBackNRdtSender::~GoBackNRdtSender() noexcept {
    delete[] sndBuffer;
}

bool GoBackNRdtSender::getWaitingState() {
    return nextSeqNum >= base + N || resending;
}

bool GoBackNRdtSender::send(const Message &message) {
    if(getWaitingState()){
        return false;
    }

    Packet mSndPkt;
    mSndPkt.acknum = -1;            // ignore
    mSndPkt.seqnum = nextSeqNum;
    mSndPkt.checksum = 0;
    memcpy(mSndPkt.payload, message.data, sizeof(message.data));

    mSndPkt.checksum = pUtils->calculateCheckSum(mSndPkt);
    sndBuffer[nextSeqNum % N] = mSndPkt;
    pUtils->printPacket("[GBN   SENDER] sending packet delivered by app layer", mSndPkt);
    pns->sendToNetworkLayer(RECEIVER, mSndPkt);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

    if(base == nextSeqNum){
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);			//启动发送方定时器
    }

    nextSeqNum++;
    return true;
}

void GoBackNRdtSender::receive(const Packet &ackPkt) {
    if(base <= ackPkt.acknum){
        if (pUtils->calculateCheckSum(ackPkt) != ackPkt.checksum) {
            pUtils->printPacket("[GBN   SENDER] ACK packet received was broken", ackPkt);
            return;
        }

        pns->stopTimer(SENDER, base);
        base = ackPkt.acknum + 1;

        pUtils->printPacket("[GBN   SENDER] ACK packet received, moving sending window", ackPkt);
        std::cout << "[GBN   SENDER] current window:" << std::endl << "[";
        for (size_t i = 0; i < N - 1; i++)
        {
            std::cout << i + base << " ";
        }

        std::cout << base + N - 1 << "]" << std::endl;
        if(base < nextSeqNum)
            pns->startTimer(SENDER, Configuration::TIME_OUT, base);
    }
}

void GoBackNRdtSender::timeoutHandler(int seqNum) {
    if (!resending) {
        resending = true;

        for (int i = base; i < nextSeqNum; i++) {
            pUtils->printPacket("[GBN   SENDER] timeout occurred, resending packet", sndBuffer[i % N]);
            pns->sendToNetworkLayer(RECEIVER, sndBuffer[i % N]);
        }
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);
        resending = false;
    }
}