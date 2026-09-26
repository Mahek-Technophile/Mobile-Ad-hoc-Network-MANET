#include "../include/Mobility.hpp"
#include <algorithm>

namespace manet {

MobilityProfile::MobilityProfile(MobilityModel model, double nodeSpeed, double arenaW, double arenaH)
    : modelType(model), speed(nodeSpeed), arenaWidth(arenaW), arenaHeight(arenaH) {}

void MobilityProfile::updateNodePosition(Coordinate& currentPos, double dt, std::mt19937& rng) {
    if (modelType == MobilityModel::STATIC) {
        return; // Fixed station does not change coordinates
    }

    if (modelType == MobilityModel::DIRECTED_MISSION) {
        // Move towards target waypoint at specified speed
        double dist = currentPos.distanceTo(targetWaypoint);
        if (dist <= 0.5) {
            // Reached destination, become stationary
            velocity = Velocity(0.0, 0.0);
            return;
        }

        double moveDistance = speed * dt;
        if (moveDistance >= dist) {
            currentPos = targetWaypoint;
            velocity = Velocity(0.0, 0.0);
        } else {
            double dirX = (targetWaypoint.x - currentPos.x) / dist;
            double dirY = (targetWaypoint.y - currentPos.y) / dist;
            velocity.vx = dirX * speed;
            velocity.vy = dirY * speed;
            currentPos.x += velocity.vx * dt;
            currentPos.y += velocity.vy * dt;
        }
        return;
    }

    if (modelType == MobilityModel::RANDOM_WAYPOINT) {
        if (pauseTimeRemaining > 0.0) {
            pauseTimeRemaining -= dt;
            velocity = Velocity(0.0, 0.0);
            return;
        }

        double dist = currentPos.distanceTo(targetWaypoint);
        if (dist <= 1.0 || targetWaypoint.x == 0.0) {
            // Pick a new random waypoint inside disaster arena bounds
            std::uniform_real_distribution<double> distX(10.0, arenaWidth - 10.0);
            std::uniform_real_distribution<double> distY(10.0, arenaHeight - 10.0);
            std::uniform_real_distribution<double> distPause(0.5, 2.0);

            targetWaypoint = Coordinate(distX(rng), distY(rng));
            pauseTimeRemaining = distPause(rng);
            velocity = Velocity(0.0, 0.0);
            return;
        }

        // Steer towards target waypoint
        double dirX = (targetWaypoint.x - currentPos.x) / dist;
        double dirY = (targetWaypoint.y - currentPos.y) / dist;
        velocity.vx = dirX * speed;
        velocity.vy = dirY * speed;

        currentPos.x += velocity.vx * dt;
        currentPos.y += velocity.vy * dt;

        // Clamp to arena boundary walls
        currentPos.x = std::max(0.0, std::min(arenaWidth, currentPos.x));
        currentPos.y = std::max(0.0, std::min(arenaHeight, currentPos.y));
    }
}

} // namespace manet
