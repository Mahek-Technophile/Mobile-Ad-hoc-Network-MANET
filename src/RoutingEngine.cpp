#include "../include/RoutingEngine.hpp"
#include "../include/Node.hpp"
#include <iostream>
#include <queue>
#include <limits>
#include <unordered_map>

namespace manet {

// -------------------------------------------------------------
// Composite Multi-Metric Link Cost Calculation
// Cost = w_d * (distance / Range) + w_e * (1.0 - EnergyRatio) + w_t * (1.0 - TrustScore)
// -------------------------------------------------------------
double RoutingEngine::computeLinkCost(double distance, double txRange, 
                                      double targetResidualEnergyRatio, double targetTrustScore) const {
    if (targetResidualEnergyRatio <= 0.001) {
        return 999999.0; // Node is battery-depleted, impassable
    }
    if (targetTrustScore < 0.40) {
        return 999999.0; // Node is flagged malicious/blackhole, shunned
    }

    double normalizedDist = std::min(1.0, distance / txRange);
    double energyPenalty = (1.0 - targetResidualEnergyRatio); // 0.0 for full battery, 1.0 for near empty
    double trustPenalty = (1.0 - targetTrustScore);           // 0.0 for perfect trust, 0.6 for marginal

    double totalCost = (weights.distanceWeight * normalizedDist) +
                       (weights.energyWeight * energyPenalty) +
                       (weights.trustWeight * trustPenalty);

    // Minimum base hop cost of 0.1 to avoid 0-cost loops
    return std::max(0.1, totalCost * 10.0);
}

void RoutingEngine::recalculateRoutes(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime) {
    if (algorithmType == RoutingAlgorithm::LINK_STATE) {
        executeLinkStateDijkstra(nodes, currentTime);
    } else {
        executeDistanceVector(nodes, currentTime);
    }
}

void RoutingEngine::executeLinkStateDijkstra(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime) {
    std::unordered_map<std::string, std::shared_ptr<Node>> ipToNode;
    for (const auto& node : nodes) {
        ipToNode[node->getIp()] = node;
        for (const auto& [nid, nbr] : node->getNeighbors()) {
            node->getArpTable().addOrUpdate(nbr.ipAddress, nbr.macAddress, currentTime);
        }
    }

    for (const auto& srcNode : nodes) {
        if (srcNode->getBattery().isDepleted()) {
            continue; // Depleted node cannot route
        }

        std::unordered_map<std::string, double> dist;
        std::unordered_map<std::string, std::string> prev;
        std::unordered_map<std::string, int> hops;

        for (const auto& n : nodes) {
            dist[n->getIp()] = std::numeric_limits<double>::infinity();
            hops[n->getIp()] = 0;
        }

        dist[srcNode->getIp()] = 0.0;

        using QueueElement = std::pair<double, std::string>;
        std::priority_queue<QueueElement, std::vector<QueueElement>, std::greater<QueueElement>> pq;
        pq.push({0.0, srcNode->getIp()});

        while (!pq.empty()) {
            auto [currentDist, currentIp] = pq.top();
            pq.pop();

            if (currentDist > dist[currentIp]) continue;

            auto currentNode = ipToNode[currentIp];
            if (!currentNode || currentNode->getBattery().isDepleted()) continue;

            for (const auto& [nbrId, nbrInfo] : currentNode->getNeighbors()) {
                auto targetIt = ipToNode.find(nbrInfo.ipAddress);
                if (targetIt == ipToNode.end()) continue;
                auto targetNode = targetIt->second;
                if (!targetNode || targetNode->getBattery().isDepleted()) continue;

                // Evaluate composite multi-metric cost considering distance, energy, and trust
                double linkCost = computeLinkCost(nbrInfo.distance, currentNode->getRange(),
                                                  targetNode->getBattery().getRemainingRatio(),
                                                  targetNode->getTrust().getTrustScore());

                if (linkCost >= 900000.0) continue; // Shunned or dead

                double newDist = currentDist + linkCost;

                if (newDist < dist[nbrInfo.ipAddress]) {
                    dist[nbrInfo.ipAddress] = newDist;
                    prev[nbrInfo.ipAddress] = currentIp;
                    hops[nbrInfo.ipAddress] = hops[currentIp] + 1;
                    pq.push({newDist, nbrInfo.ipAddress});
                }
            }
        }

        // Reconstruct next-hop for each destination
        for (const auto& destNode : nodes) {
            if (destNode->getIp() == srcNode->getIp()) continue;

            if (dist[destNode->getIp()] < 100000.0) {
                std::string curr = destNode->getIp();
                while (prev.find(curr) != prev.end() && prev[curr] != srcNode->getIp()) {
                    curr = prev[curr];
                }

                std::string nextHopIp = curr;
                uint32_t nextHopMac = 0;
                srcNode->getArpTable().lookup(nextHopIp, nextHopMac);

                RouteEntry entry;
                entry.destinationIp = destNode->getIp();
                entry.nextHopIp = nextHopIp;
                entry.nextHopMac = nextHopMac;
                entry.hopCount = hops[destNode->getIp()];
                entry.cost = dist[destNode->getIp()];
                entry.lastUpdated = currentTime;

                srcNode->getRoutingTable().addOrUpdateRoute(entry);
            } else {
                srcNode->getRoutingTable().removeRoute(destNode->getIp());
            }
        }
    }
}

void RoutingEngine::executeDistanceVector(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime) {
    for (const auto& node : nodes) {
        if (node->getBattery().isDepleted()) continue;

        for (const auto& [nid, nbr] : node->getNeighbors()) {
            node->getArpTable().addOrUpdate(nbr.ipAddress, nbr.macAddress, currentTime);

            auto nbrNode = [&]() -> std::shared_ptr<Node> {
                for (const auto& n : nodes) if (n->getId() == nid) return n;
                return nullptr;
            }();

            if (!nbrNode || nbrNode->getBattery().isDepleted()) continue;

            double cost = computeLinkCost(nbr.distance, node->getRange(),
                                          nbrNode->getBattery().getRemainingRatio(),
                                          nbrNode->getTrust().getTrustScore());
            if (cost >= 900000.0) continue;

            RouteEntry entry;
            entry.destinationIp = nbr.ipAddress;
            entry.nextHopIp = nbr.ipAddress;
            entry.nextHopMac = nbr.macAddress;
            entry.hopCount = 1;
            entry.cost = cost;
            entry.lastUpdated = currentTime;
            node->getRoutingTable().addOrUpdateRoute(entry);
        }
    }

    bool changed = true;
    size_t iterations = 0;
    while (changed && iterations < nodes.size()) {
        changed = false;
        iterations++;

        for (const auto& u : nodes) {
            if (u->getBattery().isDepleted()) continue;

            for (const auto& [vId, nbr] : u->getNeighbors()) {
                auto v = [&]() -> std::shared_ptr<Node> {
                    for (const auto& n : nodes) if (n->getId() == vId) return n;
                    return nullptr;
                }();

                if (!v || v->getBattery().isDepleted()) continue;

                double linkCost = computeLinkCost(nbr.distance, u->getRange(),
                                                  v->getBattery().getRemainingRatio(),
                                                  v->getTrust().getTrustScore());
                if (linkCost >= 900000.0) continue;

                for (const auto& [destIp, vRoute] : v->getRoutingTable().getAllRoutes()) {
                    if (destIp == u->getIp()) continue;

                    double newCost = linkCost + vRoute.cost;
                    int newHops = vRoute.hopCount + 1;

                    RouteEntry currentEntry;
                    bool exists = u->getRoutingTable().getRoute(destIp, currentEntry);

                    if (!exists || newCost < currentEntry.cost) {
                        RouteEntry updated;
                        updated.destinationIp = destIp;
                        updated.nextHopIp = nbr.ipAddress;
                        updated.nextHopMac = nbr.macAddress;
                        updated.hopCount = newHops;
                        updated.cost = newCost;
                        updated.lastUpdated = currentTime;
                        u->getRoutingTable().addOrUpdateRoute(updated);
                        changed = true;
                    }
                }
            }
        }
    }
}

} // namespace manet
