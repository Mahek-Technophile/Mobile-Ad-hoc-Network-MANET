#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace manet {

// Educational simulation of node trustworthiness and packet forwarding integrity
class TrustManager {
private:
    double trustScore{1.0};         // Ranges between 0.0 (completely untrusted) and 1.0 (fully trusted)
    double rewardAlpha{0.05};       // Increment when downstream forward is confirmed
    double penaltyBeta{0.30};       // Decrement when downstream node drops/refuses packet
    double minTrustedThreshold{0.40}; // Below this score, the node is flagged untrustworthy

    uint32_t packetsSuccessfullyForwarded{0};
    uint32_t packetsDroppedMaliciously{0};

    bool isMaliciousDropNode{false}; // Simulation flag: does this node actively act as a greyhole/blackhole?
    double dropProbability{0.0};     // Probability of dropping packets (0.0 = honest, 1.0 = full blackhole)

public:
    explicit TrustManager(double initialTrust = 1.0) : trustScore(initialTrust) {}

    // Configure simulated malicious/drop behavior for testing
    void setMaliciousBehavior(bool isMalicious, double dropProb = 0.5) {
        isMaliciousDropNode = isMalicious;
        dropProbability = dropProb;
    }

    bool isMalicious() const { return isMaliciousDropNode; }
    double getDropProbability() const { return dropProbability; }

    // Trust updates based on observed forwarding behavior
    void recordForwardSuccess() {
        packetsSuccessfullyForwarded++;
        trustScore = std::min(1.0, trustScore + rewardAlpha);
    }

    void recordPacketDrop() {
        packetsDroppedMaliciously++;
        trustScore = std::max(0.0, trustScore - penaltyBeta);
    }

    void setTrust(double score) {
        trustScore = std::max(0.0, std::min(1.0, score));
    }

    double getTrustScore() const { return trustScore; }
    bool isTrustworthy() const { return trustScore >= minTrustedThreshold; }

    uint32_t getSuccessfulForwardCount() const { return packetsSuccessfullyForwarded; }
    uint32_t getDroppedCount() const { return packetsDroppedMaliciously; }
};

} // namespace manet
