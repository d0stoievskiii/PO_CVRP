#include "helpers.hpp"


int euclideanDistance(const Node& a, const Node& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;

    return static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
}

std::vector<std::vector<int>> buildDistanceMatrix(const CVRPInstance& instance) {
    int n = instance.dimension;

    std::vector<std::vector<int>> dist(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist[i][j] = euclideanDistance(instance.nodes[i], instance.nodes[j]);
        }
    }

    return dist;
}

bool validateSolution(
    const CVRPInstance& instance,
    const Solution& sol
) {
    int n = instance.dimension;
    int depotId = instance.depotId;

    std::vector<int> visitCount(n + 1, 0);

    for (const auto& route : sol.routes) {
        int load = 0;

        for (int customerId : route.customers) {
            if (customerId < 1 || customerId > n) {
                std::cerr << "Cliente invalido: " << customerId << "\n";
                return false;
            }

            if (customerId == depotId) {
                std::cerr << "Deposito apareceu como cliente na rota\n";
                return false;
            }

            visitCount[customerId]++;

            int index = customerId - 1;
            load += instance.nodes[index].demand;
        }

        if (load > instance.capacity) {
            std::cerr << "Rota excede capacidade. Carga: "
                      << load << ", capacidade: "
                      << instance.capacity << "\n";
            return false;
        }
    }

    for (int id = 1; id <= n; id++) {
        if (id == depotId) {
            continue;
        }

        if (visitCount[id] != 1) {
            std::cerr << "Cliente " << id
                      << " visitado " << visitCount[id]
                      << " vezes\n";
            return false;
        }
    }

    return true;
}