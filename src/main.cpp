#include "../include/Common.hpp"
#include "../include/DisasterScenario.hpp"
#include <iostream>
#include <string>

using namespace manet;

void printBanner() {
    std::cout << "\n";
    std::cout << "========================================================================================\n";
    std::cout << "       MANET-SAFE: SECURE, ADAPTIVE & ENERGY-AWARE MANET DISASTER SIMULATOR\n";
    std::cout << "                 Computer Networks & Data Communication Project\n";
    std::cout << "========================================================================================\n";
}

void printMenu() {
    std::cout << "\n---------------------------- DEMONSTRATION & TEST MENU -----------------------------\n";
    std::cout << " [1] Run Complete 18-Step Disaster Management Scenario (Stage 11 Full Mission)\n";
    std::cout << " [2] Run Benchmark Comparative Experiments & Output Tables (Stage 13 Viva Results)\n";
    std::cout << " [3] Test Error Control (CRC-16, Checksum & Hamming Code Single-Bit Correction)\n";
    std::cout << " [4] Test MAC Layer Contention (CSMA/CA Backoff vs ALOHA Collision)\n";
    std::cout << " [5] Test Transport QoS & Traffic Shaping (Token Bucket vs Leaky Bucket)\n";
    std::cout << " [6] Test Context-Aware Service Discovery (Find Nearest Trusted Medical Unit)\n";
    std::cout << " [0] Exit Simulator\n";
    std::cout << "------------------------------------------------------------------------------------\n";
    std::cout << "Enter selection (or press Enter for default Full Disaster Mission): ";
}

int main(int argc, char* argv[]) {
    printBanner();

    // If an argument is provided via command line, execute non-interactively
    int choice = 1;
    if (argc > 1) {
        choice = std::stoi(argv[1]);
    } else {
        std::cout << "\n[AUTO-RUN DEMONSTRATION MODE]\n";
        std::cout << "Running Complete Disaster Management Mission Scenario (Option 1)...\n";
    }

    switch (choice) {
        case 1:
            DisasterScenario::runFullDisasterScenario();
            std::cout << "\n>>> Now generating Academic Benchmark Experiment Tables (Option 2) <<<\n";
            DisasterScenario::runComparativeExperiments();
            break;
        case 2:
            DisasterScenario::runComparativeExperiments();
            break;
        default:
            DisasterScenario::runFullDisasterScenario();
            DisasterScenario::runComparativeExperiments();
            break;
    }

    std::cout << "\n>>> ALL SIMULATION MILESTONES COMPLETED SUCCESSFULLY! <<<\n";
    return 0;
}
