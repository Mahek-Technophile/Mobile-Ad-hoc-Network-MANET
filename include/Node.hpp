#pragma once

#include "Common.hpp"
#include "Mobility.hpp"
#include "NetworkLayer.hpp"
#include "Battery.hpp"
#include "TrustManager.hpp"
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <random>

namespace manet {

// Information regarding a discovered 1-hop physical neighbor
struct NeighborInfo {
    int nodeId{0};
    std::string ipAddress;
    uint32_t macAddress{0};
    Coordinate position;
    double distance{0.0};
    double lastHeardTime{0.0};
    double residualEnergyRatio{1.0}; // Advertised battery level
    double trustScore{1.0};          // Evaluated trust score
};

class Node {
private:
    int id;
    std::string ip;
    uint32_t mac;
    NodeType role;
    Coordinate pos;
    double range;

    // Unit I: Mobility Model Profile
    MobilityProfile mobilityProfile;

    // Unit III & V: Energy & Trust Models
    Battery battery;
    TrustManager trust;

    // Unit III: Layer 3 & ARP
    ArpTable arpTable;
    RoutingTable routingTable;

    // Table of active 1-hop wireless neighbors
    std::unordered_map<int, NeighborInfo> neighborTable;

    // Frame queues
    std::queue<MacFrame> rxQueue;
    std::queue<MacFrame> txQueue;

    // Running performance statistics
    uint32_t packetsSent{0};
    uint32_t packetsReceived{0};
    uint32_t packetsForwarded{0};
    uint32_t packetsDropped{0};

public:
    Node(int nodeId, std::string ipAddress, uint32_t macAddress, 
         NodeType nodeRole, Coordinate initialPos, double txRange = 50.0, double initialJoules = 1000.0);

    // Getters
    int getId() const { return id; }
    const std::string& getIp() const { return ip; }
    uint32_t getMac() const { return mac; }
    NodeType getRole() const { return role; }
    Coordinate getPosition() const { return pos; }
    double getRange() const { return range; }

    uint32_t getPacketsSent() const { return packetsSent; }
    uint32_t getPacketsReceived() const { return packetsReceived; }
    uint32_t getPacketsForwarded() const { return packetsForwarded; }
    uint32_t getPacketsDropped() const { return packetsDropped; }
    const std::unordered_map<int, NeighborInfo>& getNeighbors() const { return neighborTable; }

    // Battery & Trust accessors
    Battery& getBattery() { return battery; }
    const Battery& getBattery() const { return battery; }

    TrustManager& getTrust() { return trust; }
    const TrustManager& getTrust() const { return trust; }

    // Mobility
    void setPosition(Coordinate newPos) { pos = newPos; }
    void setMobilityProfile(const MobilityProfile& prof) { mobilityProfile = prof; }
    MobilityProfile& getMobilityProfile() { return mobilityProfile; }
    void updateMobility(double dt, std::mt19937& rng);

    // Layer 2 Neighbor Management
    void addOrUpdateNeighbor(const NeighborInfo& info);
    std::vector<int> purgeExpiredNeighbors(double currentTime, double timeoutSeconds = 5.0);
    bool hasNeighbor(int targetNodeId) const;

    // Layer 3 Routing & ARP
    ArpTable& getArpTable() { return arpTable; }
    const ArpTable& getArpTable() const { return arpTable; }

    RoutingTable& getRoutingTable() { return routingTable; }
    const RoutingTable& getRoutingTable() const { return routingTable; }

    // Packet / Frame Queuing
    void enqueueTxFrame(const MacFrame& frame);
    bool hasPendingTx() const { return !txQueue.empty(); }
    MacFrame popTxFrame();

    void enqueueRxFrame(const MacFrame& frame);
    bool hasPendingRx() const { return !rxQueue.empty(); }
    MacFrame popRxFrame();

    void incrementForwardCount() { packetsForwarded++; }
    void incrementDropCount() { packetsDropped++; }

    // High-level packet preparation (encapsulation)
    MacFrame encapsulatePacket(const NetworkPacket& packet, uint32_t nextHopMac);

    // Logging & Diagnostics
    void printNodeSummary() const;
};

} // namespace manet
