#pragma once

#include "Common.hpp"
#include <random>
#include <string>
#include <vector>

namespace manet {

// Channel state in the wireless medium
enum class ChannelState {
    IDLE,
    BUSY,
    COLLISION
};

// Educational MAC protocol models
enum class MacProtocolType {
    CSMA_CA, // 802.11 style carrier sense + collision avoidance with backoff (Primary MANET)
    CSMA_CD, // 802.3 Ethernet style collision detection (Demonstration of why it fails on wireless)
    PURE_ALOHA // Transmit immediately without sensing (Demonstration of high collision rate)
};

class MacLayer {
private:
    MacProtocolType protocol{MacProtocolType::CSMA_CA};

    // CSMA/CA Backoff Parameters
    uint32_t cwMin{7};          // Minimum Contention Window
    uint32_t cwMax{127};        // Maximum Contention Window
    uint32_t currentCw{7};
    uint32_t backoffSlotsRemaining{0};
    uint32_t collisionCount{0};
    uint32_t backoffAttempts{0};

    double difsDuration{0.005};  // 5ms DCF Interframe Space
    double sifsDuration{0.002};  // 2ms Short Interframe Space
    double idleTimeObserved{0.0};

    bool channelSensedBusy{false};

public:
    explicit MacLayer(MacProtocolType type = MacProtocolType::CSMA_CA) : protocol(type) {}

    void setProtocol(MacProtocolType type) { protocol = type; }
    MacProtocolType getProtocol() const { return protocol; }

    // Carrier Sense: Senses whether nearby wireless medium is in use
    void senseChannel(bool isBusy, double dt);

    // Determines if MAC allows immediate frame transmission
    bool canTransmitFrame();

    // Trigger exponential backoff on collision / channel busy
    void recordCollisionOrBusy(std::mt19937& rng);

    // Reset backoff window upon successful ACK
    void recordTransmissionSuccess();

    uint32_t getCollisionCount() const { return collisionCount; }
    uint32_t getBackoffAttempts() const { return backoffAttempts; }
    uint32_t getCurrentCW() const { return currentCw; }
};

} // namespace manet
