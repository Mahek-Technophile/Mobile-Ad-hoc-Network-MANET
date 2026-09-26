#pragma once

#include "Common.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>

namespace manet {

// Represents an orthogonal wireless RF channel (e.g., 2.4GHz WiFi channels 1, 6, 11)
struct WirelessChannel {
    int channelId{1};
    double frequencyGHz{2.412}; // e.g. 2.412 GHz
    bool isOccupied{false};
    int currentTransmitterId{-1};
    Coordinate transmitterPos{0.0, 0.0};
    double txRadius{50.0};
};

class WirelessMediumManager {
private:
    std::unordered_map<int, WirelessChannel> channels;

public:
    WirelessMediumManager() {
        // Initialize 3 orthogonal, non-overlapping channels
        channels[1] = WirelessChannel{1, 2.412, false, -1, Coordinate(0, 0), 50.0};
        channels[6] = WirelessChannel{6, 2.437, false, -1, Coordinate(0, 0), 50.0};
        channels[11] = WirelessChannel{11, 2.462, false, -1, Coordinate(0, 0), 50.0};
    }

    // Check if a transmission on channelId from txPos will cause collision/interference
    bool checkInterference(int channelId, const Coordinate& txPos, double txRange, int& outConflictingNodeId) {
        auto it = channels.find(channelId);
        if (it == channels.end()) return false;

        const auto& ch = it->second;
        if (ch.isOccupied) {
            // Check spatial distance between active transmitter and candidate transmitter
            double distance = txPos.distanceTo(ch.transmitterPos);
            // Interference occurs if within collision radius (spatial reuse threshold)
            if (distance < (txRange + ch.txRadius)) {
                outConflictingNodeId = ch.currentTransmitterId;
                return true; // Co-channel interference!
            }
        }
        return false; // Spatial separation sufficient or channel idle
    }

    // Allocate transmission on channel
    void occupyChannel(int channelId, int nodeId, const Coordinate& pos, double range) {
        if (channels.find(channelId) != channels.end()) {
            channels[channelId].isOccupied = true;
            channels[channelId].currentTransmitterId = nodeId;
            channels[channelId].transmitterPos = pos;
            channels[channelId].txRadius = range;
        }
    }

    // Release transmission on channel
    void releaseChannel(int channelId) {
        if (channels.find(channelId) != channels.end()) {
            channels[channelId].isOccupied = false;
            channels[channelId].currentTransmitterId = -1;
        }
    }

    // Find best available channel with least interference
    int selectOptimalChannel(const Coordinate& txPos, double txRange) {
        for (const auto& [chId, ch] : channels) {
            int conflictId = -1;
            if (!checkInterference(chId, txPos, txRange, conflictId)) {
                return chId; // Found interference-free channel
            }
        }
        return 1; // Fallback to channel 1
    }
};

} // namespace manet
