#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace manet {

// Realistic energy consumption model for battery-powered disaster nodes
class Battery {
private:
    double maxEnergyJoules{1000.0};     // E.g., 1000 Joules full charge
    double currentEnergyJoules{1000.0};  // Current residual energy

    // Energy parameters (Joules)
    double energyTxBase{0.5};          // Base circuit energy for transmitter
    double energyTxAmpCoeff{0.001};     // Distance-squared RF amplifier coefficient (k * d^2)
    double energyRx{0.3};              // Energy to receive and decode a frame
    double energyIdlePerSec{0.05};      // Continuous idle radio listening drain

public:
    explicit Battery(double initialJoules = 1000.0)
        : maxEnergyJoules(initialJoules), currentEnergyJoules(initialJoules) {}

    // Consume energy for wireless transmission over distance d (meters)
    void consumeTx(double distanceMeters) {
        if (isDepleted()) return;
        double cost = energyTxBase + energyTxAmpCoeff * (distanceMeters * distanceMeters);
        currentEnergyJoules = std::max(0.0, currentEnergyJoules - cost);
    }

    // Consume energy for receiving a frame
    void consumeRx() {
        if (isDepleted()) return;
        currentEnergyJoules = std::max(0.0, currentEnergyJoules - energyRx);
    }

    // Consume idle power over elapsed time delta
    void consumeIdle(double dt) {
        if (isDepleted()) return;
        currentEnergyJoules = std::max(0.0, currentEnergyJoules - (energyIdlePerSec * dt));
    }

    // Direct manual override (useful for testing depletion scenarios)
    void setEnergy(double joules) {
        currentEnergyJoules = std::max(0.0, std::min(maxEnergyJoules, joules));
    }

    double getCurrentEnergy() const { return currentEnergyJoules; }
    double getMaxEnergy() const { return maxEnergyJoules; }

    // Remaining battery ratio between 0.0 (empty) and 1.0 (fully charged)
    double getRemainingRatio() const {
        if (maxEnergyJoules <= 0.0) return 0.0;
        return currentEnergyJoules / maxEnergyJoules;
    }

    bool isDepleted() const { return currentEnergyJoules <= 0.001; }
    bool isLowBattery() const { return getRemainingRatio() < 0.25; }
};

} // namespace manet
