#pragma once

#include "Common.hpp"
#include <queue>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace manet {

// -------------------------------------------------------------
// Unit IV: Traffic Shaping & Policing - Token Bucket
// Allows bursts up to bucketCapacity, refills at refillRatePerSec
// -------------------------------------------------------------
class TokenBucket {
private:
    double bucketCapacity{10.0};  // Maximum burst allowance (tokens)
    double currentTokens{10.0};   // Available tokens
    double refillRatePerSec{5.0}; // Tokens added per second
    double lastRefillTime{0.0};

public:
    explicit TokenBucket(double capacity = 10.0, double refillRate = 5.0)
        : bucketCapacity(capacity), currentTokens(capacity), refillRatePerSec(refillRate) {}

    void refill(double currentTime) {
        if (lastRefillTime <= 0.0) {
            lastRefillTime = currentTime;
            return;
        }
        double delta = currentTime - lastRefillTime;
        if (delta > 0.0) {
            currentTokens = std::min(bucketCapacity, currentTokens + (delta * refillRatePerSec));
            lastRefillTime = currentTime;
        }
    }

    // Attempt to consume 'tokensNeeded' (e.g. 1 token per packet)
    bool consume(double tokensNeeded, double currentTime) {
        refill(currentTime);
        if (currentTokens >= tokensNeeded) {
            currentTokens -= tokensNeeded;
            return true; // Token granted, transmission allowed
        }
        return false; // Conforming limit exceeded, rate-limited
    }

    double getTokens() const { return currentTokens; }
};

// -------------------------------------------------------------
// Unit IV: Traffic Shaping - Leaky Bucket
// Constant leak rate (packets per second) with a FIFO buffer
// -------------------------------------------------------------
class LeakyBucket {
private:
    size_t bufferCapacity{10};
    double leakRatePerSec{2.0}; // Exactly 2 packets per second
    double lastLeakTime{0.0};
    std::queue<NetworkPacket> queue;

public:
    explicit LeakyBucket(size_t capacity = 10, double leakRate = 2.0)
        : bufferCapacity(capacity), leakRatePerSec(leakRate) {}

    bool enqueue(const NetworkPacket& packet) {
        if (queue.size() >= bufferCapacity) {
            return false; // Buffer overflow, packet dropped
        }
        queue.push(packet);
        return true;
    }

    // Leaks packets at constant interval
    bool leak(double currentTime, NetworkPacket& outPacket) {
        if (queue.empty()) return false;

        if (lastLeakTime <= 0.0) {
            lastLeakTime = currentTime;
            outPacket = queue.front();
            queue.pop();
            return true;
        }

        double interval = 1.0 / leakRatePerSec;
        if (currentTime - lastLeakTime >= interval) {
            lastLeakTime = currentTime;
            outPacket = queue.front();
            queue.pop();
            return true;
        }
        return false;
    }

    size_t getQueueSize() const { return queue.size(); }
};

// -------------------------------------------------------------
// Unit IV: Quality of Service (QoS) Multi-Tier Priority Queue
// Strict Priority: CRITICAL -> HIGH -> NORMAL -> LOW
// -------------------------------------------------------------
class QoSQueueManager {
private:
    std::queue<NetworkPacket> criticalQueue; // Tier 0: SOS
    std::queue<NetworkPacket> highQueue;     // Tier 1: Rescue
    std::queue<NetworkPacket> normalQueue;   // Tier 2: Telemetry
    std::queue<NetworkPacket> lowQueue;      // Tier 3: Diagnostics

    size_t maxPerTierQueueSize{50};

public:
    explicit QoSQueueManager(size_t maxQueue = 50) : maxPerTierQueueSize(maxQueue) {}

    bool enqueue(const NetworkPacket& packet) {
        switch (packet.priority) {
            case PacketPriority::CRITICAL:
                if (criticalQueue.size() >= maxPerTierQueueSize) return false;
                criticalQueue.push(packet);
                break;
            case PacketPriority::HIGH:
                if (highQueue.size() >= maxPerTierQueueSize) return false;
                highQueue.push(packet);
                break;
            case PacketPriority::NORMAL:
                if (normalQueue.size() >= maxPerTierQueueSize) return false;
                normalQueue.push(packet);
                break;
            case PacketPriority::LOW:
                if (lowQueue.size() >= maxPerTierQueueSize) return false;
                lowQueue.push(packet);
                break;
        }
        return true;
    }

    // Strict priority dequeue: Always service higher priority first
    bool dequeue(NetworkPacket& outPacket) {
        if (!criticalQueue.empty()) {
            outPacket = criticalQueue.front();
            criticalQueue.pop();
            return true;
        }
        if (!highQueue.empty()) {
            outPacket = highQueue.front();
            highQueue.pop();
            return true;
        }
        if (!normalQueue.empty()) {
            outPacket = normalQueue.front();
            normalQueue.pop();
            return true;
        }
        if (!lowQueue.empty()) {
            outPacket = lowQueue.front();
            lowQueue.pop();
            return true;
        }
        return false; // All queues empty
    }

    bool hasPending() const {
        return !criticalQueue.empty() || !highQueue.empty() || 
               !normalQueue.empty() || !lowQueue.empty();
    }

    size_t getTotalQueueSize() const {
        return criticalQueue.size() + highQueue.size() + 
               normalQueue.size() + lowQueue.size();
    }
};

} // namespace manet
