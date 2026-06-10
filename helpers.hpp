#pragma once

#include "parse_instance/dotvrp.hpp"

int euclideanDistance(const Node& a, const Node& b);
std::vector<std::vector<int>> buildDistanceMatrix(const CVRPInstance& instance);

bool validateSolution(const CVRPInstance& instance, const Solution& sol);