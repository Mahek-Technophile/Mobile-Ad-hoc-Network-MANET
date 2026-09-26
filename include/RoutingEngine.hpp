#pragma once

#include "Common.hpp"
#include "NetworkLayer.hpp"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace manet {

class Node;

enum class RoutingAlgorithm {
    DISTANCE_VECTOR, // Unit III: Distributed Bellman-Ford (RIP style)
    LINK_STATE       // Unit III: Dijkstra Shortest Path (OSPF style)
};

// Weight configuration for the composite multi-metric cost function
struct CostWeights {
    double distanceWeight{0.30};    // w_d: Normalized physical distance
    double energyWeight{0.35};      // w_e: Depleted energy penalty
    double trustWeight{0.35};       // w_t: Untrustworthy node penalty

    CostWeights() = default;
    CostWeights(double wd, double we, double wt) 
        : distanceWeight(wd), energyWeight(we), trustWeight(wt) {}
};

class RoutingEngine {
private:
    RoutingAlgorithm algorithmType{RoutingAlgorithm::LINK_STATE};
    CostWeights weights;

public:
    explicit RoutingEngine(RoutingAlgorithm algo = RoutingAlgorithm::LINK_STATE, 
                           CostWeights w = CostWeights()) 
        : algorithmType(algo), weights(w) {}

    void setAlgorithm(RoutingAlgorithm algo) { algorithmType = algo; }
    RoutingAlgorithm getAlgorithm() const { return algorithmType; }
    std::string getAlgorithmName() const {
        return (algorithmType == RoutingAlgorithm::LINK_STATE) ? "Link State (Dijkstra)" : "Distance Vector (Bellman-Ford)";
    }

    void setWeights(const CostWeights& w) { weights = w; }
    const CostWeights& getWeights() const { return weights; }

    // Evaluates multi-metric link cost between two nodes
    double computeLinkCost(double distance, double txRange, double targetResidualEnergyRatio, double targetTrustScore) const;

    // Recalculates routing tables across all nodes in the network
    void recalculateRoutes(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime);

private:
    void executeLinkStateDijkstra(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime);
    void executeDistanceVector(const std::vector<std::shared_ptr<Node>>& nodes, double currentTime);
};

} // namespace manet
