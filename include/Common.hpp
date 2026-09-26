#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace manet {

// -------------------------------------------------------------
// Networking Enums
// -------------------------------------------------------------

// Unit IV: Quality of Service (QoS) Priority Levels
enum class PacketPriority : uint8_t {
    CRITICAL = 0, // Emergency SOS, Medical evacuation alert
    HIGH     = 1, // Rescue team coordination commands
    NORMAL   = 2, // Sensor data, environmental status
    LOW      = 3  // General telemetry, background pings
};

// Types of packets exchanged across layers
enum class PacketType : uint8_t {
    DATA,       // User/Application payload
    BEACON,     // Periodic 1-hop Hello for neighbor discovery
    ACK,        // Link-layer acknowledgment
    NACK,       // Negative acknowledgment (corruption/loss)
    ROUTING_LSA,// Link State Advertisement (Unit III)
    ROUTING_DV, // Distance Vector Table update (Unit III)
    SERVICE_REQ,// Application Layer: Service Discovery Query (Unit V)
    SERVICE_REP // Application Layer: Service Discovery Reply (Unit V)
};

// Functional roles of nodes in a disaster management MANET
enum class NodeType : uint8_t {
    COMMAND_CENTER, // Central tactical coordinator (high power)
    RESCUE_TEAM,    // First responders on foot
    AMBULANCE,      // Mobile medical emergency unit
    SENSOR,         // Stationary sensor (e.g. seismic/gas detector)
    MOBILE_NODE     // Displaced civilian / general volunteer
};

// -------------------------------------------------------------
// Geometric 2D Coordinate & Physical Range Math
// -------------------------------------------------------------
struct Coordinate {
    double x{0.0};
    double y{0.0};

    Coordinate() = default;
    Coordinate(double x_val, double y_val) : x(x_val), y(y_val) {}

    // Calculate Euclidean distance between two nodes in 2D space
    double distanceTo(const Coordinate& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// -------------------------------------------------------------
// Layer 3: Network Layer Packet Header & Payload
// -------------------------------------------------------------
struct NetworkPacket {
    uint32_t packetId{0};
    std::string srcIp;
    std::string destIp;
    PacketType type{PacketType::DATA};
    PacketPriority priority{PacketPriority::NORMAL};
    uint8_t ttl{16}; // Time-To-Live to prevent infinite forwarding loops
    uint16_t sequenceNumber{0};
    std::string payload;

    // Helper to format packet information as a readable string
    std::string toString() const;
};

// -------------------------------------------------------------
// Layer 2: Data Link / MAC Layer Frame
// -------------------------------------------------------------
struct MacFrame {
    uint32_t srcMac{0};
    uint32_t nextHopMac{0}; // 0xFFFFFFFF denotes wireless broadcast
    uint16_t sequenceNumber{0};
    uint16_t crc16{0};      // Unit II: CRC error detection field
    NetworkPacket networkPacket;

    static constexpr uint32_t BROADCAST_MAC = 0xFFFFFFFF;
};

// -------------------------------------------------------------
// Helper Display Utilities
// -------------------------------------------------------------
std::string priorityToString(PacketPriority priority);
std::string packetTypeToString(PacketType type);
std::string nodeTypeToString(NodeType type);

} // namespace manet
