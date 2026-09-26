#include "../include/ServiceDiscovery.hpp"
#include "../include/Node.hpp"
#include <algorithm>

namespace manet {

std::vector<ServiceMatch> ServiceDiscoveryEngine::findServices(
    const ServiceQuery& query,
    const std::vector<std::shared_ptr<Node>>& allNodes,
    const DnsResolver& dns) {

    std::vector<ServiceMatch> matches;

    for (const auto& node : allNodes) {
        // 1. Role Match
        if (node->getRole() != query.desiredRole) continue;

        // 2. Battery & Depletion Check
        double batteryRatio = node->getBattery().getRemainingRatio();
        if (node->getBattery().isDepleted() || batteryRatio < query.minBatteryRatio) continue;

        // 3. Trust Score Check
        double trust = node->getTrust().getTrustScore();
        if (trust < query.minTrustScore) continue;

        // 4. Physical Distance Check
        double dist = query.requesterPos.distanceTo(node->getPosition());
        if (dist > query.maxDistance) continue;

        // Compute composite suitability score (Higher is better)
        // Balances proximity, high battery, and high trust
        double normDistScore = 1.0 - (dist / query.maxDistance);
        double suitability = (0.40 * normDistScore) + (0.35 * batteryRatio) + (0.25 * trust);

        std::string host = "node-" + std::to_string(node->getId()) + ".disaster.net";
        dns.reverseResolve(node->getIp(), host);

        ServiceMatch match;
        match.nodeId = node->getId();
        match.ipAddress = node->getIp();
        match.hostname = host;
        match.distance = dist;
        match.residualBattery = batteryRatio;
        match.trustScore = trust;
        match.suitabilityScore = suitability;

        matches.push_back(match);
    }

    // Sort by suitability score descending (best candidate first)
    std::sort(matches.begin(), matches.end(), [](const ServiceMatch& a, const ServiceMatch& b) {
        return a.suitabilityScore > b.suitabilityScore;
    });

    return matches;
}

} // namespace manet
