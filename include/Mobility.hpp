#pragma once

#include "Common.hpp"
#include <random>

namespace manet {

// Velocity vector (meters per second)
struct Velocity {
    double vx{0.0};
    double vy{0.0};

    Velocity() = default;
    Velocity(double x_vel, double y_vel) : vx(x_vel), vy(y_vel) {}

    double getSpeed() const {
        return std::sqrt(vx * vx + vy * vy);
    }
};

// Supported mobility profiles in a disaster management environment
enum class MobilityModel {
    STATIC,          // E.g., Command Center or Fixed Seismic Sensor
    RANDOM_WAYPOINT, // E.g., Volunteers wandering or searching a sector
    DIRECTED_MISSION // E.g., Ambulance moving along a direct trajectory to an evacuation site
};

class MobilityProfile {
private:
    MobilityModel modelType{MobilityModel::STATIC};
    Velocity velocity{0.0, 0.0};
    Coordinate targetWaypoint{0.0, 0.0};
    double speed{0.0}; // m/s
    double pauseTimeRemaining{0.0};
    double arenaWidth{300.0};
    double arenaHeight{300.0};

public:
    MobilityProfile() = default;
    MobilityProfile(MobilityModel model, double nodeSpeed, double arenaW = 300.0, double arenaH = 300.0);

    // Setters & Getters
    void setModel(MobilityModel model) { modelType = model; }
    MobilityModel getModel() const { return modelType; }
    void setVelocity(const Velocity& v) { velocity = v; }
    const Velocity& getVelocity() const { return velocity; }
    void setSpeed(double s) { speed = s; }
    double getSpeed() const { return speed; }

    void setTargetWaypoint(const Coordinate& target) {
        targetWaypoint = target;
        modelType = MobilityModel::DIRECTED_MISSION;
    }

    // Step physics & update position coordinates
    void updateNodePosition(Coordinate& currentPos, double dt, std::mt19937& rng);
};

} // namespace manet
