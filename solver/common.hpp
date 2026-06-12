#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include "../parse_instance/dotvrp.hpp"
#include "../helpers.hpp"

struct Candidate {
    int customerId;
    int insertionCost;
};

struct VNDConfig {
    bool useRelocate = true;
    bool useSwap = true;
    bool useTwoOpt = true;
    bool useTwoOptInter = false;
};

struct AlgorithmVariant {
    std::string name;
    VNDConfig config;
};

int getDemand(const CVRPInstance& instance, int nodeId);

int dist(const CVRPInstance& instance, int fromId, int toId);

int calculateRouteCost(const CVRPInstance& instance, const Route& route);

int calculateRouteLoad(const CVRPInstance& instance, const Route& route);

int calculateRouteLoad(const CVRPInstance& instance, const std::vector<int>& route);

int calculateSolutionCost(const CVRPInstance& instance, const Solution& solution);

void updateRouteInfo(const CVRPInstance& instance, Route& route);

void updateSolutionInfo(const CVRPInstance& instance, Solution& solution);

void removeEmptyRoutes(Solution& solution);

int routeCostWithCustomers(const CVRPInstance& instance, const std::vector<int>& customers);