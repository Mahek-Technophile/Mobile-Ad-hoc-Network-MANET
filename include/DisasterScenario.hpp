#pragma once

#include "Common.hpp"
#include "Node.hpp"
#include "Mobility.hpp"
#include "SimulatorEngine.hpp"
#include "ServiceDiscovery.hpp"
#include "ErrorControl.hpp"
#include "ReliableTransport.hpp"
#include "TrafficShaper.hpp"
#include "WirelessMedium.hpp"
#include <iostream>
#include <vector>
#include <memory>

namespace manet {

class DisasterScenario {
public:
    // Executes the complete 18-step realistic disaster management simulation
    static void runFullDisasterScenario();

    // Runs experimental benchmark comparison test
    static void runComparativeExperiments();
};

} // namespace manet
