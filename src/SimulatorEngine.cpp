#include "../include/SimulatorEngine.hpp"
#include <iostream>
#include <iomanip>

namespace manet {

SimulatorEngine::SimulatorEngine(double dt, unsigned int randomSeed) 
    : timeStep(dt), rng(randomSeed), routingEngine(RoutingAlgorithm::LINK_STATE) {}

void SimulatorEngine::addNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
}

std::shared_ptr<Node> SimulatorEngine::getNodeById(int id) const {
    for (const auto& node : nodes) {
        if (node->getId() == id) return node;
    }
    return nullptr;
}

std::shared_ptr<Node> SimulatorEngine::getNodeByIp(const std::string& ip) const {
    for (const auto& node : nodes) {
        if (node->getIp() == ip) return node;
    }
    return nullptr;
}

void SimulatorEngine::runRoutingUpdates() {
    routingEngine.recalculateRoutes(nodes, currentTime);
}

void SimulatorEngine::executeNeighborDiscovery() {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i]->getBattery().isDepleted()) continue;

        for (size_t j = 0; j < nodes.size(); ++j) {
            if (i == j) continue;

            const auto& nodeA = nodes[i];
            const auto& nodeB = nodes[j];

            if (nodeB->getBattery().isDepleted()) continue;

            double dist = nodeA->getPosition().distanceTo(nodeB->getPosition());

            if (dist <= nodeA->getRange()) {
                bool wasAlreadyNeighbor = nodeA->hasNeighbor(nodeB->getId());

                NeighborInfo info;
                info.nodeId = nodeB->getId();
                info.ipAddress = nodeB->getIp();
                info.macAddress = nodeB->getMac();
                info.position = nodeB->getPosition();
                info.distance = dist;
                info.lastHeardTime = currentTime;
                info.residualEnergyRatio = nodeB->getBattery().getRemainingRatio();
                info.trustScore = nodeB->getTrust().getTrustScore();

                nodeA->addOrUpdateNeighbor(info);

                if (!wasAlreadyNeighbor) {
                    totalNewLinksEstablished++;
                }
            }
        }

        std::vector<int> broken = nodes[i]->purgeExpiredNeighbors(currentTime, 0.5);
        for (int brokenId : broken) {
            totalLinkBreaksDetected++;
            auto brokenNode = getNodeById(brokenId);
            if (brokenNode) {
                nodes[i]->getRoutingTable().removeRoutesViaNextHop(brokenNode->getIp());
            }
        }
    }
}

bool SimulatorEngine::transmitFrame(int senderNodeId, const MacFrame& frame) {
    auto sender = getNodeById(senderNodeId);
    if (!sender || sender->getBattery().isDepleted()) return false;

    totalTransmissionsAttempted++;
    bool atLeastOneReceived = false;

    bool isBroadcast = (frame.nextHopMac == MacFrame::BROADCAST_MAC);

    for (const auto& receiver : nodes) {
        if (receiver->getId() == senderNodeId || receiver->getBattery().isDepleted()) continue;

        double dist = sender->getPosition().distanceTo(receiver->getPosition());

        if (dist <= sender->getRange()) {
            if (isBroadcast || receiver->getMac() == frame.nextHopMac) {
                // Sender consumes transmission battery
                sender->getBattery().consumeTx(dist);

                // Receiver consumes reception battery
                receiver->getBattery().consumeRx();

                receiver->enqueueRxFrame(frame);
                atLeastOneReceived = true;
                totalFramesDelivered++;
            }
        }
    }

    if (!atLeastOneReceived && !isBroadcast) {
        totalFramesDroppedOutOfRange++;
        return false;
    }

    return atLeastOneReceived;
}

// -------------------------------------------------------------
// Unit III & V: Multi-Hop Forwarding with Trust & Energy Tracking
// -------------------------------------------------------------
bool SimulatorEngine::routeMultiHopPacket(const NetworkPacket& originalPacket) {
    auto srcNode = getNodeByIp(originalPacket.srcIp);
    auto destNode = getNodeByIp(originalPacket.destIp);

    if (!srcNode || !destNode) {
        std::cerr << "[ROUTING ERROR] Unknown source or destination IP\n";
        return false;
    }

    std::cout << "\n>>> [MULTI-HOP ROUTING INITIATED (" << routingEngine.getAlgorithmName() << ")]\n";
    std::cout << "Origin: Node [" << srcNode->getId() << "] (" << srcNode->getIp() 
              << ") ---> Final Destination: Node [" << destNode->getId() << "] (" << destNode->getIp() << ")\n";

    NetworkPacket currentPkt = originalPacket;
    auto currentNode = srcNode;
    int hopCount = 0;

    while (currentNode && currentNode->getIp() != destNode->getIp()) {
        if (currentNode->getBattery().isDepleted()) {
            std::cout << "   [BATTERY DEPLETED] Node [" << currentNode->getId() << "] ran out of power! Dropping packet.\n";
            currentNode->incrementDropCount();
            return false;
        }

        if (currentPkt.ttl == 0) {
            std::cout << "   [DROP] ICMP Time Exceeded: TTL reached 0 at Node [" << currentNode->getId() << "]!\n";
            currentNode->incrementDropCount();
            return false;
        }

        RouteEntry nextRoute;
        bool hasRoute = currentNode->getRoutingTable().getRoute(destNode->getIp(), nextRoute);

        if (!hasRoute) {
            std::cout << "   [ROUTING ERROR] No viable route to destination " << destNode->getIp() 
                      << " in Node [" << currentNode->getId() << "] table! Path broken or node avoided.\n";
            return false;
        }

        auto nextHopNode = getNodeByIp(nextRoute.nextHopIp);
        if (!nextHopNode || nextHopNode->getBattery().isDepleted()) {
            std::cout << "   [ROUTING ERROR] Next hop IP " << nextRoute.nextHopIp << " is unresolvable or dead!\n";
            return false;
        }

        // Unit V: Check if next-hop acts as an untrusted greyhole / blackhole
        if (nextHopNode->getTrust().isMalicious()) {
            std::uniform_real_distribution<double> randProb(0.0, 1.0);
            if (randProb(rng) < nextHopNode->getTrust().getDropProbability()) {
                std::cout << "   [MALICIOUS PACKET DROP DETECTED] Next-Hop Node [" << nextHopNode->getId() 
                          << "] dropped the packet unprovoked!\n";
                nextHopNode->getTrust().recordPacketDrop();
                nextHopNode->incrementDropCount();
                std::cout << "   └── Trust Score of Node [" << nextHopNode->getId() 
                          << "] reduced to " << std::fixed << std::setprecision(2) 
                          << nextHopNode->getTrust().getTrustScore() << "\n";
                // Trigger route recalculation to avoid this compromised node
                runRoutingUpdates();
                return false;
            }
        }

        currentPkt.ttl--;
        hopCount++;
        totalHopsTraversed++;

        MacFrame frame = currentNode->encapsulatePacket(currentPkt, nextRoute.nextHopMac);
        bool delivered = transmitFrame(currentNode->getId(), frame);

        if (!delivered) {
            std::cout << "   [LINK FAILURE] Transmission from Node [" << currentNode->getId() 
                      << "] to next-hop Node [" << nextHopNode->getId() << "] failed!\n";
            return false;
        }

        std::cout << "   └── Hop " << hopCount << ": Node [" << currentNode->getId() 
                  << "] -> Node [" << nextHopNode->getId() << "] (MAC: 0x" 
                  << std::hex << nextRoute.nextHopMac << std::dec
                  << " | Batt: " << std::fixed << std::setprecision(1) << (nextHopNode->getBattery().getRemainingRatio() * 100.0) << "%"
                  << " | Trust: " << std::setprecision(2) << nextHopNode->getTrust().getTrustScore()
                  << " | Link Dist: " << currentNode->getPosition().distanceTo(nextHopNode->getPosition()) << "m)\n";

        if (nextHopNode->getIp() != destNode->getIp()) {
            nextHopNode->incrementForwardCount();
            nextHopNode->getTrust().recordForwardSuccess();
        }

        currentNode = nextHopNode;
    }

    totalPacketsRoutedMultiHop++;
    std::cout << "   [PACKET DELIVERED] Reached Destination Node [" << destNode->getId() 
              << "] in " << hopCount << " hops! Payload: \"" << currentPkt.payload << "\"\n";
    return true;
}

bool SimulatorEngine::sendEmergencyMessage(int srcId, int destId, PacketPriority priority, const std::string& message) {
    auto srcNode = getNodeById(srcId);
    auto destNode = getNodeById(destId);

    if (!srcNode || !destNode) return false;

    NetworkPacket pkt;
    pkt.packetId = nextPacketId++;
    pkt.srcIp = srcNode->getIp();
    pkt.destIp = destNode->getIp();
    pkt.type = PacketType::DATA;
    pkt.priority = priority;
    pkt.ttl = 16;
    pkt.sequenceNumber = 1;
    pkt.payload = message;

    return routeMultiHopPacket(pkt);
}

bool SimulatorEngine::sendIcmpPing(int srcId, int destId) {
    auto srcNode = getNodeById(srcId);
    auto destNode = getNodeById(destId);

    if (!srcNode || !destNode) return false;

    std::cout << "\n[ICMP PING] Pinging " << destNode->getIp() << " from " << srcNode->getIp() << " with 32 bytes of data:\n";

    NetworkPacket pingPkt;
    pingPkt.packetId = nextPacketId++;
    pingPkt.srcIp = srcNode->getIp();
    pingPkt.destIp = destNode->getIp();
    pingPkt.type = PacketType::DATA;
    pingPkt.priority = PacketPriority::LOW;
    pingPkt.ttl = 16;
    pingPkt.sequenceNumber = 100;
    pingPkt.payload = "ICMP_ECHO_REQUEST";

    bool success = routeMultiHopPacket(pingPkt);
    if (success) {
        std::cout << "[ICMP PING REPLY] Received ECHO_REPLY from " << destNode->getIp() 
                  << " (Ping Round-Trip Successful!)\n";
    } else {
        std::cout << "[ICMP PING FAILED] Request timed out / Destination Host Unreachable.\n";
    }
    return success;
}

void SimulatorEngine::step() {
    currentTime += timeStep;

    for (auto& node : nodes) {
        node->updateMobility(timeStep, rng);
    }

    executeNeighborDiscovery();
    runRoutingUpdates();
}

void SimulatorEngine::runForDuration(double durationSeconds) {
    double targetTime = currentTime + durationSeconds;
    while (currentTime < targetTime) {
        step();
    }
}

void SimulatorEngine::printTopologyReport() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "               MANET-SAFE: CURRENT DISASTER ZONE TOPOLOGY REPORT (t = " 
              << std::fixed << std::setprecision(2) << currentTime << "s)\n";
    std::cout << "========================================================================================\n";
    for (const auto& node : nodes) {
        node->printNodeSummary();
    }
    std::cout << "========================================================================================\n";
}

void SimulatorEngine::printAllRoutingTables() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "         MANET-SAFE: NETWORK LAYER ROUTING TABLES [" << routingEngine.getAlgorithmName() << "]\n";
    std::cout << "========================================================================================\n";
    for (const auto& node : nodes) {
        if (!node->getBattery().isDepleted()) {
            node->getRoutingTable().printRoutingTable(node->getId());
        }
    }
}

void SimulatorEngine::printSimulationStats() const {
    std::cout << "\n--- SIMULATION AGGREGATE STATISTICS ---\n";
    std::cout << "Active Routing Algorithm      : " << routingEngine.getAlgorithmName() << "\n";
    std::cout << "Total Elapsed Simulation Time : " << std::fixed << std::setprecision(2) << currentTime << "s\n";
    std::cout << "Packets Successfully Routed   : " << totalPacketsRoutedMultiHop << "\n";
    std::cout << "Total Network Hops Traversed  : " << totalHopsTraversed << "\n";
    std::cout << "Dynamic Link Breaks Detected  : " << totalLinkBreaksDetected << "\n";
    std::cout << "New Dynamic Links Established : " << totalNewLinksEstablished << "\n";
    std::cout << "--------------------------------------\n\n";
}

} // namespace manet
