#include "../include/NetworkLayer.hpp"
#include <iostream>
#include <vector>

namespace manet {

// IPv4Address parsing and CIDR operations
IPv4Address::IPv4Address(const std::string& ipStr, uint8_t prefix) : prefixLength(prefix) {
    uint32_t a = 0, b = 0, c = 0, d = 0;
    char dot1, dot2, dot3;
    std::istringstream iss(ipStr);
    if (iss >> a >> dot1 >> b >> dot2 >> c >> dot3 >> d) {
        address = (a << 24) | (b << 16) | (c << 8) | d;
    }
}

std::string IPv4Address::toString() const {
    std::ostringstream oss;
    oss << ((address >> 24) & 0xFF) << "."
        << ((address >> 16) & 0xFF) << "."
        << ((address >> 8) & 0xFF) << "."
        << (address & 0xFF);
    return oss.str();
}

uint32_t IPv4Address::getSubnetMask() const {
    if (prefixLength == 0) return 0;
    if (prefixLength >= 32) return 0xFFFFFFFF;
    return ~((1ULL << (32 - prefixLength)) - 1);
}

uint32_t IPv4Address::getNetworkAddress() const {
    return address & getSubnetMask();
}

bool IPv4Address::isInSameSubnet(const IPv4Address& other) const {
    return getNetworkAddress() == (other.address & getSubnetMask());
}

// ARP Table
void ArpTable::addOrUpdate(const std::string& ip, uint32_t mac, double time) {
    table[ip] = ArpEntry{ip, mac, time};
}

bool ArpTable::lookup(const std::string& ip, uint32_t& outMac) const {
    auto it = table.find(ip);
    if (it != table.end()) {
        outMac = it->second.macAddress;
        return true;
    }
    return false;
}

void ArpTable::printTable() const {
    std::cout << "--- ARP CACHE TABLE ---\n";
    for (const auto& [ip, entry] : table) {
        std::cout << "IP: " << std::left << std::setw(15) << ip
                  << " -> MAC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << entry.macAddress << std::dec << std::setfill(' ')
                  << " (Updated @" << entry.timestamp << "s)\n";
    }
}

// Routing Table
void RoutingTable::addOrUpdateRoute(const RouteEntry& entry) {
    entries[entry.destinationIp] = entry;
}

bool RoutingTable::getRoute(const std::string& destinationIp, RouteEntry& outRoute) const {
    auto it = entries.find(destinationIp);
    if (it != entries.end()) {
        outRoute = it->second;
        return true;
    }
    return false;
}

void RoutingTable::removeRoute(const std::string& destinationIp) {
    entries.erase(destinationIp);
}

void RoutingTable::removeRoutesViaNextHop(const std::string& nextHopIp) {
    auto it = entries.begin();
    while (it != entries.end()) {
        if (it->second.nextHopIp == nextHopIp) {
            it = entries.erase(it);
        } else {
            ++it;
        }
    }
}

void RoutingTable::printRoutingTable(int nodeId) const {
    std::cout << "--- Routing Table for Node [" << nodeId << "] ---\n";
    std::cout << std::left << std::setw(16) << "Destination"
              << std::setw(16) << "Next Hop IP"
              << std::setw(14) << "Next Hop MAC"
              << std::setw(10) << "Hops"
              << std::setw(10) << "Cost" << "\n";
    std::cout << "------------------------------------------------------------\n";
    for (const auto& [dest, entry] : entries) {
        std::cout << std::left << std::setw(16) << entry.destinationIp
                  << std::setw(16) << entry.nextHopIp
                  << "0x" << std::hex << std::setw(10) << std::setfill('0') << entry.nextHopMac << std::dec << std::setfill(' ')
                  << std::setw(10) << entry.hopCount
                  << std::fixed << std::setprecision(1) << entry.cost << "\n";
    }
    std::cout << "------------------------------------------------------------\n";
}

} // namespace manet
