#include "common.hpp"

int getDemand(const CVRPInstance& instance, int nodeId) {
    return instance.nodes[nodeId - 1].demand;
}

int dist(const CVRPInstance& instance, int fromId, int toId) {
    return instance.distance[fromId - 1][toId - 1];
}

int calculateRouteLoad(const CVRPInstance& instance, const Route& route) {
    int load = 0;

    for (int customerId : route.customers) {
        load += getDemand(instance, customerId);
    }

    return load;
}

int calculateRouteLoad(const CVRPInstance& instance, const std::vector<int>& route) {
    int load = 0;

    for (int customerId : route) {
        load += getDemand(instance, customerId);
    }

    return load;
}

int calculateRouteCost(const CVRPInstance& instance, const Route& route) {
    int depotId = instance.depotId;

    if (route.customers.empty()) {
        return 0;
    }

    int cost = 0;

    cost += dist(instance, depotId, route.customers.front());

    for (size_t i = 0; i + 1 < route.customers.size(); i++) {
        cost += dist(instance, route.customers[i], route.customers[i + 1]);
    }

    cost += dist(instance, route.customers.back(), depotId);

    return cost;
}

int calculateSolutionCost(const CVRPInstance& instance, const Solution& solution) {
    int total = 0;

    for (const Route& route : solution.routes) {
        total += calculateRouteCost(instance, route);
    }

    return total;
}

void updateRouteInfo(const CVRPInstance& instance, Route& route) {
    route.load = calculateRouteLoad(instance, route);
    route.cost = calculateRouteCost(instance, route);
}

void updateSolutionInfo(const CVRPInstance& instance, Solution& solution) {
    solution.totalCost = 0;

    for (Route& route : solution.routes) {
        updateRouteInfo(instance, route);
        solution.totalCost += route.cost;
    }
}

void removeEmptyRoutes(Solution& solution) {
    solution.routes.erase(
        std::remove_if(
            solution.routes.begin(),
            solution.routes.end(),
            [](const Route& r) {
                return r.customers.empty();
            }
        ),
        solution.routes.end()
    );
}

int routeCostWithCustomers(
    const CVRPInstance& instance,
    const std::vector<int>& customers
) {
    int depotId = instance.depotId;

    if (customers.empty()) {
        return 0;
    }

    int cost = dist(instance, depotId, customers.front());

    for (size_t i = 0; i + 1 < customers.size(); i++) {
        cost += dist(instance, customers[i], customers[i + 1]);
    }

    cost += dist(instance, customers.back(), depotId);

    return cost;
}