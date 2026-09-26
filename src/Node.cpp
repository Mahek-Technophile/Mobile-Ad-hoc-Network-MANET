#include "../include/Node.hpp"
#include <iostream>
#include <iomanip>

namespace manet {

Node::Node(int nodeId, std::string ipAddress, uint32_t macAddress, 
           NodeType nodeRole, Coordinate initialPos, double txRange, double initialJoules)
    : id(nodeId), ip(std::move(ipAddress)), mac(macAddress), 
      role(nodeRole), pos(initialPos), range(txRange), battery(initialJoules) {}

void Node::updateMobility(double dt, std::mt19937& rng) {
    if (battery.isDepleted()) return; // Dead node cannot move
    mobilityProfile.updateNodePosition(pos, dt, rng);
    battery.consumeIdle(dt);
}

void Node::addOrUpdateNeighbor(const NeighborInfo& info) {
    neighborTable[info.nodeId] = info;
}

std::vector<int> Node::purgeExpiredNeighbors(double currentTime, double timeoutSeconds) {
    std::vector<int> brokenNeighbors;
    auto it = neighborTable.begin();
    while (it != neighborTable.end()) {
        if (currentTime - it->second.lastHeardTime > timeoutSeconds) {
            brokenNeighbors.push_back(it->first);
            it = neighborTable.erase(it);
        } else {
            ++it;
        }
    }
    return brokenNeighbors;
}

bool Node::hasNeighbor(int targetNodeId) const {
    return neighborTable.find(targetNodeId) != neighborTable.end();
}

void Node::enqueueTxFrame(const MacFrame& frame) {
    txQueue.push(frame);
    packetsSent++;
}

MacFrame Node::popTxFrame() {
    if (txQueue.empty()) {
        return MacFrame{};
    }
    MacFrame frame = txQueue.front();
    txQueue.pop();
    return frame;
}

void Node::enqueueRxFrame(const MacFrame& frame) {
    rxQueue.push(frame);
    packetsReceived++;
}

MacFrame Node::popRxFrame() {
    if (rxQueue.empty()) {
        return MacFrame{};
    }
    MacFrame frame = rxQueue.front();
    rxQueue.pop();
    return frame;
}

MacFrame Node::encapsulatePacket(const NetworkPacket& packet, uint32_t nextHopMac) {
    MacFrame frame;
    frame.srcMac = this->mac;
    frame.nextHopMac = nextHopMac;
    frame.sequenceNumber = packet.sequenceNumber;
    frame.crc16 = 0;
    frame.networkPacket = packet;
    return frame;
}

void Node::printNodeSummary() const {
    std::cout << "Node [" << id << "] | " << std::left << std::setw(15) << nodeTypeToString(role)
              << " | IP: " << std::setw(10) << ip
              << " | Pos: (" << std::fixed << std::setprecision(1) << pos.x << ", " << pos.y << ")"
              << " | Batt: " << std::setw(5) << std::fixed << std::setprecision(1) 
              << (battery.getRemainingRatio() * 100.0) << "%"
              << " | Trust: " << std::setw(4) << std::fixed << std::setprecision(2) << trust.getTrustScore()
              << (trust.isTrustworthy() ? " [OK]" : " [UNTRUSTED]")
              << (battery.isDepleted() ? " [DEAD]" : "")
              << " | Nbrs: " << neighborTable.size() << "\n";
}

} // namespace manet
