#pragma once

#include "Common.hpp"
#include "Node.hpp"
#include "Mobility.hpp"
#include "RoutingEngine.hpp"
#include <vector>
#include <memory>
#include <string>
#include <random>

namespace manet {

class SimulatorEngine {
private:
    double currentTime{0.0};
    double timeStep{0.1};
    uint32_t nextPacketId{1001};

    std::mt19937 rng;

    std::vector<std::shared_ptr<Node>> nodes;
    RoutingEngine routingEngine;

    // Simulation-wide aggregate statistics
    uint32_t totalTransmissionsAttempted{0};
    uint32_t totalFramesDelivered{0};
    uint32_t totalFramesDroppedOutOfRange{0};
    uint32_t totalPacketsRoutedMultiHop{0};
    uint32_t totalHopsTraversed{0};
    uint32_t totalLinkBreaksDetected{0};
    uint32_t totalNewLinksEstablished{0};

public:
    explicit SimulatorEngine(double dt = 0.1, unsigned int randomSeed = 42);

    // Node management
    void addNode(std::shared_ptr<Node> node);
    std::shared_ptr<Node> getNodeById(int id) const;
    std::shared_ptr<Node> getNodeByIp(const std::string& ip) const;
    const std::vector<std::shared_ptr<Node>>& getAllNodes() const { return nodes; }

    // Routing configuration
    void setRoutingAlgorithm(RoutingAlgorithm algo) { routingEngine.setAlgorithm(algo); }
    RoutingAlgorithm getRoutingAlgorithm() const { return routingEngine.getAlgorithm(); }
    std::string getRoutingAlgorithmName() const { return routingEngine.getAlgorithmName(); }
    void runRoutingUpdates();

    // Time & stepping
    double getCurrentTime() const { return currentTime; }
    void step();
    void runForDuration(double durationSeconds);

    // Dynamic Wireless Medium & Neighbor Discovery (Unit I & II)
    void executeNeighborDiscovery();

    // Direct Wireless Transmission Simulation
    bool transmitFrame(int senderNodeId, const MacFrame& frame);

    // Unit III: Multi-Hop Packet Forwarding & Routing
    bool routeMultiHopPacket(const NetworkPacket& packet);

    // High-Level Helper to dispatch an emergency SOS
    bool sendEmergencyMessage(int srcId, int destId, PacketPriority priority, const std::string& message);

    // Unit III: ICMP-like Ping Utility
    bool sendIcmpPing(int srcId, int destId);

    // Visual Status & Report
    void printTopologyReport() const;
    void printAllRoutingTables() const;
    void printSimulationStats() const;
};

} // namespace manet
