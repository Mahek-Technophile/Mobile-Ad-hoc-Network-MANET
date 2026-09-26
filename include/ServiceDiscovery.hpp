#pragma once

#include "Common.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

namespace manet {

class Node; // Forward declaration

// -------------------------------------------------------------
// Unit V: DHCP-like Dynamic Address Allocation Simulation
// -------------------------------------------------------------
class DhcpServer {
private:
    std::string subnetPrefix{"10.0.0."};
    uint8_t nextHostId{10}; // Assigns 10.0.0.10, 10.0.0.11, etc.
    std::unordered_map<uint32_t, std::string> macToIpLease;

public:
    std::string allocateIp(uint32_t macAddress) {
        auto it = macToIpLease.find(macAddress);
        if (it != macToIpLease.end()) {
            return it->second; // Return existing lease
        }
        std::string assignedIp = subnetPrefix + std::to_string(nextHostId++);
        macToIpLease[macAddress] = assignedIp;
        return assignedIp;
    }

    void releaseIp(uint32_t macAddress) {
        macToIpLease.erase(macAddress);
    }
};

// -------------------------------------------------------------
// Unit V: DNS-like Name Resolution Simulation
// Maps names like "ambulance-1.disaster.net" to "10.0.0.3"
// -------------------------------------------------------------
class DnsResolver {
private:
    std::unordered_map<std::string, std::string> hostnameToIp;
    std::unordered_map<std::string, std::string> ipToHostname;

public:
    void registerHost(const std::string& hostname, const std::string& ip) {
        hostnameToIp[hostname] = ip;
        ipToHostname[ip] = hostname;
    }

    bool resolve(const std::string& hostname, std::string& outIp) const {
        auto it = hostnameToIp.find(hostname);
        if (it != hostnameToIp.end()) {
            outIp = it->second;
            return true;
        }
        return false;
    }

    bool reverseResolve(const std::string& ip, std::string& outHostname) const {
        auto it = ipToHostname.find(ip);
        if (it != ipToHostname.end()) {
            outHostname = it->second;
            return true;
        }
        return false;
    }
};

// -------------------------------------------------------------
// Unit V: Context-Aware Service Discovery Engine
// Queries available disaster services considering:
// 1. Role (Ambulance, Rescue, Sensor)
// 2. Physical Distance
// 3. Battery Residual Level (> threshold)
// 4. Trust Score (> threshold)
// -------------------------------------------------------------
struct ServiceQuery {
    NodeType desiredRole;
    Coordinate requesterPos;
    double maxDistance{200.0};
    double minBatteryRatio{0.25}; // Need at least 25% battery
    double minTrustScore{0.60};   // Must be reputable
};

struct ServiceMatch {
    int nodeId{0};
    std::string ipAddress;
    std::string hostname;
    double distance{0.0};
    double residualBattery{0.0};
    double trustScore{0.0};
    double suitabilityScore{0.0}; // Higher is better
};

class ServiceDiscoveryEngine {
public:
    static std::vector<ServiceMatch> findServices(
        const ServiceQuery& query,
        const std::vector<std::shared_ptr<Node>>& allNodes,
        const DnsResolver& dns);
};

} // namespace manet
