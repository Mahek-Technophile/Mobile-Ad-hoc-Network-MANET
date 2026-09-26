#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace manet {

// -------------------------------------------------------------
// Unit III: IPv4 Addressing & CIDR Subnetting Representation
// -------------------------------------------------------------
struct IPv4Address {
    uint32_t address{0}; // 32-bit unsigned representation
    uint8_t prefixLength{24}; // Default /24 subnet

    IPv4Address() = default;
    explicit IPv4Address(const std::string& ipStr, uint8_t prefix = 24);

    std::string toString() const;
    uint32_t getSubnetMask() const;
    uint32_t getNetworkAddress() const;
    bool isInSameSubnet(const IPv4Address& other) const;

    bool operator==(const IPv4Address& other) const { return address == other.address; }
    bool operator!=(const IPv4Address& other) const { return address != other.address; }
};

// -------------------------------------------------------------
// Unit III: ARP (Address Resolution Protocol) Table
// Maps simulated IPv4 address to 48/32-bit simulated MAC address
// -------------------------------------------------------------
struct ArpEntry {
    std::string ipAddress;
    uint32_t macAddress{0};
    double timestamp{0.0};
};

class ArpTable {
private:
    std::unordered_map<std::string, ArpEntry> table;

public:
    void addOrUpdate(const std::string& ip, uint32_t mac, double time);
    bool lookup(const std::string& ip, uint32_t& outMac) const;
    void printTable() const;
};

// -------------------------------------------------------------
// Unit III: Routing Table Entry
// -------------------------------------------------------------
struct RouteEntry {
    std::string destinationIp;
    std::string nextHopIp;
    uint32_t nextHopMac{0};
    int hopCount{0};
    double cost{1.0};
    double lastUpdated{0.0};
};

class RoutingTable {
private:
    std::unordered_map<std::string, RouteEntry> entries;

public:
    void addOrUpdateRoute(const RouteEntry& entry);
    bool getRoute(const std::string& destinationIp, RouteEntry& outRoute) const;
    void removeRoute(const std::string& destinationIp);
    void removeRoutesViaNextHop(const std::string& nextHopIp);
    const std::unordered_map<std::string, RouteEntry>& getAllRoutes() const { return entries; }
    void printRoutingTable(int nodeId) const;
};

} // namespace manet
