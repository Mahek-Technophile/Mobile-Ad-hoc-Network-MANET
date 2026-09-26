#include "../include/ReliableTransport.hpp"
#include <iostream>

namespace manet {

bool ReliableTransport::sendData(const std::string& data, double currentTime, SlidingWindowPacket& outPkt) {
    // Check if sliding window is full: [baseSeqNum, baseSeqNum + windowSize)
    if (senderBuffer.size() >= windowSize) {
        return false; // Window is full, sender must wait for ACKs
    }

    SlidingWindowPacket pkt;
    pkt.seqNumber = nextSeqNum++;
    pkt.payload = data;
    pkt.crc16 = ErrorControl::computeCRC16(data);
    pkt.isAcked = false;
    pkt.sendTime = currentTime;
    pkt.retransmissionAttempts = 0;

    senderBuffer[pkt.seqNumber] = pkt;
    outPkt = pkt;
    totalPacketsSent++;
    return true;
}

bool ReliableTransport::receiveFrame(const SlidingWindowPacket& frame, uint16_t& outAckSeq, bool& outIsAck) {
    outAckSeq = frame.seqNumber;

    // Unit II: CRC-16 Verification
    bool crcValid = ErrorControl::verifyCRC16(frame.payload, frame.crc16);

    if (!crcValid) {
        // Frame is corrupted by wireless interference/noise -> send NACK
        outIsAck = false;
        totalNacksReceived++;
        std::cout << "   [RECEIVER CRC ERROR] Frame Seq " << frame.seqNumber 
                  << " payload corrupted! Checksum mismatch. Sending NACK.\n";
        return false;
    }

    // Frame is intact -> send ACK
    outIsAck = true;

    if (mode == ArqMode::SELECTIVE_REPEAT) {
        // Selective Repeat buffers frames that arrive out of order within receiver window
        receiverBuffer[frame.seqNumber] = frame;

        // Slide receiver window base if consecutive frames are ready
        while (receiverBuffer.find(expectedReceiverBase) != receiverBuffer.end()) {
            receiverBuffer.erase(expectedReceiverBase);
            expectedReceiverBase++;
        }
    } else {
        // Go-Back-N only accepts strictly in-order frames
        if (frame.seqNumber == expectedReceiverBase) {
            expectedReceiverBase++;
        }
    }

    return true;
}

void ReliableTransport::processAck(uint16_t ackSeq, bool isAck) {
    if (isAck) {
        totalAcksReceived++;
        if (mode == ArqMode::SELECTIVE_REPEAT) {
            // In Selective Repeat, mark ONLY the specific sequence number as acknowledged
            if (senderBuffer.find(ackSeq) != senderBuffer.end()) {
                senderBuffer[ackSeq].isAcked = true;
                std::cout << "   [ACK RECEIVED] Frame Seq " << ackSeq << " confirmed by receiver.\n";
            }

            // Slide sender window base past consecutively ACKed packets
            while (!senderBuffer.empty() && senderBuffer.begin()->second.isAcked) {
                senderBuffer.erase(senderBuffer.begin());
            }
        } else {
            // In Go-Back-N, ACKs are cumulative
            auto it = senderBuffer.begin();
            while (it != senderBuffer.end() && it->first <= ackSeq) {
                it = senderBuffer.erase(it);
            }
            std::cout << "   [GBN CUMULATIVE ACK] All frames up to Seq " << ackSeq << " confirmed.\n";
        }
    } else {
        // NACK received: in Selective Repeat, trigger immediate selective retransmission
        totalNacksReceived++;
        std::cout << "   [NACK RECEIVED] Retransmission requested for Frame Seq " << ackSeq << ".\n";
    }
}

std::vector<SlidingWindowPacket> ReliableTransport::checkTimeouts(double currentTime) {
    std::vector<SlidingWindowPacket> retransmitList;

    if (mode == ArqMode::SELECTIVE_REPEAT) {
        // In Selective Repeat, retransmit ONLY individual timed-out un-ACKed packets
        for (auto& [seq, pkt] : senderBuffer) {
            if (!pkt.isAcked && (currentTime - pkt.sendTime > timeoutSeconds)) {
                pkt.sendTime = currentTime;
                pkt.retransmissionAttempts++;
                totalRetransmissions++;
                retransmitList.push_back(pkt);
            }
        }
    } else {
        // In Go-Back-N, if base times out, retransmit ALL packets currently in the window
        if (!senderBuffer.empty() && (currentTime - senderBuffer.begin()->second.sendTime > timeoutSeconds)) {
            for (auto& [seq, pkt] : senderBuffer) {
                pkt.sendTime = currentTime;
                pkt.retransmissionAttempts++;
                totalRetransmissions++;
                retransmitList.push_back(pkt);
            }
        }
    }

    return retransmitList;
}

} // namespace manet
