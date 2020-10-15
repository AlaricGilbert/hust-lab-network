#include "SelectiveRepeatRdtSender.h"
#include "Global.h"
SelectiveRepeatRdtSender::SelectiveRepeatRdtSender(int N) :N(N), base(0), nextSeqNum(0), sndBuffer(new Packet[N]), resending(false) {}

SelectiveRepeatRdtSender::~SelectiveRepeatRdtSender() noexcept {
    delete[] sndBuffer;
}

bool SelectiveRepeatRdtSender::getWaitingState() {
    return nextSeqNum >= base + N || resending;
}

bool SelectiveRepeatRdtSender::send(const Message& message) {
    if (getWaitingState()) {
        return false;
    }

    Packet mSndPkt;
    mSndPkt.acknum = -1;            // ignore
    mSndPkt.seqnum = nextSeqNum;
    mSndPkt.checksum = 0;
    memcpy(mSndPkt.payload, message.data, sizeof(message.data));

    mSndPkt.checksum = pUtils->calculateCheckSum(mSndPkt);
    sndBuffer[nextSeqNum % N] = mSndPkt;

    pUtils->printPacket("[SR   SENDER] sending packet delivered by app layer", mSndPkt);
    pns->sendToNetworkLayer(RECEIVER, mSndPkt);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);			//启动发送方定时器
    

    nextSeqNum++;
    return true;
}

void SelectiveRepeatRdtSender::receive(const Packet& ackPkt) {
    if (pUtils->calculateCheckSum(ackPkt) == ackPkt.checksum) {
        pUtils->printPacket("[SR   SENDER] ACK packet received, sending window may be moved", ackPkt);
        pns->stopTimer(SENDER, ackPkt.acknum);
        sndBuffer[ackPkt.acknum % N].acknum = ackPkt.acknum;
    }
    if (ackPkt.acknum == base) {
        while (sndBuffer[base % N].acknum == base) {
            base += 1;
        }
        std::cout << "[SR   SENDER] sending window moved, current sending window: " << std::endl << "[";
        for (size_t i = 0; i < N - 1; i++)
        {
            std::cout << base + i << " ";
        }

        std::cout << base + N - 1 << "]" << std::endl;
    }
}

void SelectiveRepeatRdtSender::timeoutHandler(int seqNum) {
    if (!resending) {
        resending = true;

        pUtils->printPacket("[SR   SENDER] timeout occurred, resending packet", sndBuffer[seqNum % N]);
        pns->sendToNetworkLayer(RECEIVER, sndBuffer[seqNum % N]);
        pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
        resending = false;
    }
}