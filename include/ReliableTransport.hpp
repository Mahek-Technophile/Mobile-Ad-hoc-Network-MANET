#pragma once

#include "Common.hpp"
#include "ErrorControl.hpp"
#include <vector>
#include <map>
#include <iostream>

namespace manet {

// ARQ Reliability Mechanism Mode
enum class ArqMode {
    SELECTIVE_REPEAT, // Primary Unit II Sliding Window ARQ
    GO_BACK_N         // Unit II Benchmark Comparison
};

struct SlidingWindowPacket {
    uint16_t seqNumber{0};
    std::string payload;
    uint16_t crc16{0};
    bool isAcked{false};
    double sendTime{0.0};
    int retransmissionAttempts{0};
};

class ReliableTransport {
private:
    ArqMode mode{ArqMode::SELECTIVE_REPEAT};
    uint16_t windowSize{4};
    uint16_t nextSeqNum{0};
    uint16_t baseSeqNum{0};
    double timeoutSeconds{1.0};

    // Sender transmission window buffer
    std::map<uint16_t, SlidingWindowPacket> senderBuffer;

    // Receiver window buffer (Selective Repeat receives out of order)
    std::map<uint16_t, SlidingWindowPacket> receiverBuffer;
    uint16_t expectedReceiverBase{0};

    // Statistics
    uint32_t totalPacketsSent{0};
    uint32_t totalRetransmissions{0};
    uint32_t totalAcksReceived{0};
    uint32_t totalNacksReceived{0};

public:
    explicit ReliableTransport(ArqMode m = ArqMode::SELECTIVE_REPEAT, uint16_t winSize = 4)
        : mode(m), windowSize(winSize) {}

    void setMode(ArqMode m) { mode = m; }
    ArqMode getMode() const { return mode; }
    std::string getModeName() const {
        return (mode == ArqMode::SELECTIVE_REPEAT) ? "Selective Repeat" : "Go-Back-N";
    }

    // Sender submits a message to be buffered in the sliding window
    bool sendData(const std::string& data, double currentTime, SlidingWindowPacket& outPkt);

    // Receiver processes incoming frame, checks CRC-16, returns ACK or NACK
    bool receiveFrame(const SlidingWindowPacket& frame, uint16_t& outAckSeq, bool& outIsAck);

    // Sender processes incoming ACK / NACK
    void processAck(uint16_t ackSeq, bool isAck);

    // Check timeouts and get list of packets needing retransmission
    std::vector<SlidingWindowPacket> checkTimeouts(double currentTime);

    uint32_t getTotalRetransmissions() const { return totalRetransmissions; }
    uint32_t getTotalSent() const { return totalPacketsSent; }
};

} // namespace manet
