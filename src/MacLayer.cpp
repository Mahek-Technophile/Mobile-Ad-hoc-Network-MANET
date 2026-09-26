#include "../include/MacLayer.hpp"
#include <iostream>
#include <algorithm>

namespace manet {

void MacLayer::senseChannel(bool isBusy, double dt) {
    channelSensedBusy = isBusy;

    if (isBusy) {
        idleTimeObserved = 0.0;
    } else {
        idleTimeObserved += dt;
        // If channel has been idle for DIFS, decrement backoff slots
        if (idleTimeObserved >= difsDuration && backoffSlotsRemaining > 0) {
            backoffSlotsRemaining--;
        }
    }
}

bool MacLayer::canTransmitFrame() {
    if (protocol == MacProtocolType::PURE_ALOHA) {
        // Pure ALOHA transmits blindly without sensing the carrier
        return true;
    }

    if (protocol == MacProtocolType::CSMA_CD) {
        // CSMA/CD transmits if carrier is idle, but cannot detect collision during half-duplex Tx
        return !channelSensedBusy;
    }

    // Default CSMA/CA (802.11 MANET style)
    if (channelSensedBusy) {
        return false;
    }

    // Must wait for DIFS idle duration and clear any backoff slots
    return (idleTimeObserved >= difsDuration) && (backoffSlotsRemaining == 0);
}

void MacLayer::recordCollisionOrBusy(std::mt19937& rng) {
    collisionCount++;
    backoffAttempts++;

    // Binary Exponential Backoff: CW = min(CW_max, (CW + 1) * 2 - 1)
    currentCw = std::min(cwMax, (currentCw + 1) * 2 - 1);

    std::uniform_int_distribution<uint32_t> dist(0, currentCw);
    backoffSlotsRemaining = dist(rng);

    std::cout << "   [MAC CSMA/CA BACKOFF] Contention window expanded to CW=" 
              << currentCw << ". Chosen backoff slots: " << backoffSlotsRemaining << "\n";
}

void MacLayer::recordTransmissionSuccess() {
    // Reset contention window to minimum upon successful ACK
    currentCw = cwMin;
    backoffSlotsRemaining = 0;
}

} // namespace manet
