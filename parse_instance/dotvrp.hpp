#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

struct Node {
    int id = 0;
    double x = 0.0;
    double y = 0.0;
    int demand = 0;
};

struct CVRPInstance {
    std::string name;
    std::string type;
    std::string edgeWeightType;

    int dimension = 0;
    int capacity = 0;
    int depotId = 1;

    std::vector<Node> nodes;
    std::vector<std::vector<int>> distance;
};

struct Route {
    std::vector<int> customers;
    int load;
    int cost;
};

struct Solution {
    std::vector<Route> routes;
    int totalCost;
};


static std::string trim(const std::string& s);

static std::pair<std::string, std::string> parseKeyValue(const std::string& line);

CVRPInstance readVRP(const std::string& filename);

Solution readSolution(const std::string& filename);

int routeCost(const std::vector<int>& route, const std::vector<std::vector<int>>& dist, int depotId);

int solutionCost(const Solution& sol, const std::vector<std::vector<int>>& dist, int depotId);