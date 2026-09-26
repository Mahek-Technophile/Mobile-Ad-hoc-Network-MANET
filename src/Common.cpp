#include "../include/Common.hpp"

namespace manet {

std::string priorityToString(PacketPriority priority) {
    switch (priority) {
        case PacketPriority::CRITICAL: return "CRITICAL[SOS]";
        case PacketPriority::HIGH:     return "HIGH[RESCUE]";
        case PacketPriority::NORMAL:   return "NORMAL[TELEMETRY]";
        case PacketPriority::LOW:      return "LOW[STATUS]";
        default:                       return "UNKNOWN";
    }
}

std::string packetTypeToString(PacketType type) {
    switch (type) {
        case PacketType::DATA:        return "DATA";
        case PacketType::BEACON:      return "BEACON";
        case PacketType::ACK:         return "ACK";
        case PacketType::NACK:        return "NACK";
        case PacketType::ROUTING_LSA: return "LSA";
        case PacketType::ROUTING_DV:  return "DV_UPDATE";
        case PacketType::SERVICE_REQ: return "SRV_REQ";
        case PacketType::SERVICE_REP: return "SRV_REP";
        default:                      return "UNKNOWN";
    }
}

std::string nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::COMMAND_CENTER: return "Command Center";
        case NodeType::RESCUE_TEAM:    return "Rescue Squad";
        case NodeType::AMBULANCE:      return "Ambulance Unit";
        case NodeType::SENSOR:         return "Seismic Sensor";
        case NodeType::MOBILE_NODE:    return "Field Volunteer";
        default:                       return "Node";
    }
}

std::string NetworkPacket::toString() const {
    std::ostringstream oss;
    oss << "[Pkt #" << packetId << " | " << packetTypeToString(type)
        << " | " << priorityToString(priority)
        << " | Src: " << srcIp << " -> Dst: " << destIp
        << " | TTL: " << static_cast<int>(ttl)
        << " | Seq: " << sequenceNumber
        << " | Payload: \"" << payload << "\"]";
    return oss.str();
}

} // namespace manet
