#include "dotvrp.hpp"

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::pair<std::string, std::string> parseKeyValue(const std::string& line) {
    size_t pos = line.find(':');

    if (pos == std::string::npos) {
        std::istringstream iss(line);
        std::string key, value;
        iss >> key;
        std::getline(iss, value);
        return {trim(key), trim(value)};
    }

    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    return {key, value};
}

CVRPInstance readVRP(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo: " + filename);
    }

    CVRPInstance instance;

    enum class Section {
        NONE,
        NODE_COORD,
        DEMAND,
        DEPOT
    };

    Section section = Section::NONE;

    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        if (line == "EOF") {
            break;
        }

        if (line == "NODE_COORD_SECTION") {
            section = Section::NODE_COORD;
            continue;
        }

        if (line == "DEMAND_SECTION") {
            section = Section::DEMAND;
            continue;
        }

        if (line == "DEPOT_SECTION") {
            section = Section::DEPOT;
            continue;
        }

        if (section == Section::NONE) {
            auto [key, value] = parseKeyValue(line);

            if (key == "NAME") {
                instance.name = value;
            } else if (key == "TYPE") {
                instance.type = value;
            } else if (key == "EDGE_WEIGHT_TYPE") {
                instance.edgeWeightType = value;
            } else if (key == "DIMENSION") {
                instance.dimension = std::stoi(value);
                instance.nodes.resize(instance.dimension);
            } else if (key == "CAPACITY") {
                instance.capacity = std::stoi(value);
            }

            continue;
        }

        if (section == Section::NODE_COORD) {
            std::istringstream iss(line);

            int id;
            double x, y;

            iss >> id >> x >> y;

            int index = id - 1;

            if (index < 0 || index >= instance.dimension) {
                throw std::runtime_error("ID de no invalido em NODE_COORD_SECTION");
            }

            instance.nodes[index].id = id;
            instance.nodes[index].x = x;
            instance.nodes[index].y = y;

            continue;
        }

        if (section == Section::DEMAND) {
            std::istringstream iss(line);

            int id;
            int demand;

            iss >> id >> demand;

            int index = id - 1;

            if (index < 0 || index >= instance.dimension) {
                throw std::runtime_error("ID de no invalido em DEMAND_SECTION");
            }

            instance.nodes[index].id = id;
            instance.nodes[index].demand = demand;

            continue;
        }

        if (section == Section::DEPOT) {
            int depotId = std::stoi(line);

            if (depotId == -1) {
                section = Section::NONE;
                continue;
            }

            instance.depotId = depotId;
            continue;
        }
    }

    return instance;
}

Solution readSolution(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo: " + filename);
    }

    Solution sol;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        if (line.rfind("Route", 0) == 0) {
            size_t colon = line.find(':');

            if (colon == std::string::npos) {
                continue;
            }

            std::string routeText = line.substr(colon + 1);
            std::istringstream iss(routeText);

            Route route;
            int customer;

            while (iss >> customer) {
                route.customers.push_back(customer + 1);
            }

            sol.routes.push_back(route);
        } else if (line.rfind("Cost", 0) == 0) {
            std::istringstream iss(line);

            std::string word;
            int cost;

            iss >> word >> cost;
            sol.totalCost = cost;
        }
    }

    return sol;
}

int routeCost(
    const std::vector<int>& route,
    const std::vector<std::vector<int>>& dist,
    int depotId
) {
    if (route.empty()) {
        return 0;
    }

    int depotIndex = depotId - 1;

    int cost = 0;

    int firstCustomerIndex = route.front() - 1;
    cost += dist[depotIndex][firstCustomerIndex];

    for (size_t i = 0; i + 1 < route.size(); i++) {
        int from = route[i] - 1;
        int to = route[i + 1] - 1;

        cost += dist[from][to];
    }

    int lastCustomerIndex = route.back() - 1;
    cost += dist[lastCustomerIndex][depotIndex];

    return cost;
}

int solutionCost(
    const Solution& sol,
    const std::vector<std::vector<int>>& dist,
    int depotId
) {
    int total = 0;

    for (const auto& route : sol.routes) {
        total += routeCost(route.customers, dist, depotId);
    }

    return total;
}