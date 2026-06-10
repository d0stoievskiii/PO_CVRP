#include "vnd.hpp"

Solution bestRelocate(
    const Solution& current,
    const CVRPInstance& instance
) {
    Solution best = current;

    int bestCost = current.totalCost;

    for (size_t originRouteIndex = 0; originRouteIndex < current.routes.size(); originRouteIndex++) {
        const Route& originRoute = current.routes[originRouteIndex];

        for (size_t originPos = 0; originPos < originRoute.customers.size(); originPos++) {
            int customer = originRoute.customers[originPos];
            int customerDemand = getDemand(instance, customer);

            for (size_t destRouteIndex = 0; destRouteIndex < current.routes.size(); destRouteIndex++) {
                const Route& destRoute = current.routes[destRouteIndex];

                for (size_t destPos = 0; destPos <= destRoute.customers.size(); destPos++) {

                    // Evita movimento que não muda nada na mesma rota.
                    if (originRouteIndex == destRouteIndex) {
                        if (destPos == originPos || destPos == originPos + 1) {
                            continue;
                        }
                    }

                    // Checagem de capacidade quando move entre rotas diferentes.
                    if (originRouteIndex != destRouteIndex) {
                        if (destRoute.load + customerDemand > instance.capacity) {
                            continue;
                        }
                    }

                    Solution neighbor = current;

                    Route& neighborOrigin = neighbor.routes[originRouteIndex];
                    Route& neighborDest = neighbor.routes[destRouteIndex];

                    // Remove cliente da origem.
                    neighborOrigin.customers.erase(
                        neighborOrigin.customers.begin() + originPos
                    );

                    // Se for a mesma rota e removemos uma posição anterior,
                    // o índice de inserção precisa ser ajustado.
                    size_t adjustedDestPos = destPos;

                    if (originRouteIndex == destRouteIndex && destPos > originPos) {
                        adjustedDestPos--;
                    }

                    // Cuidado: depois do erase, referências podem continuar válidas
                    // porque não alteramos o vetor routes, só o vetor customers.
                    neighborDest.customers.insert(
                        neighborDest.customers.begin() + adjustedDestPos,
                        customer
                    );

                    updateSolutionInfo(instance, neighbor);
                    removeEmptyRoutes(neighbor);
                    updateSolutionInfo(instance, neighbor);

                    if (neighbor.totalCost < bestCost) {
                        best = neighbor;
                        bestCost = neighbor.totalCost;
                    }
                }
            }
        }
    }

    return best;
}

Solution bestSwap(
    const Solution& current,
    const CVRPInstance& instance
) {
    Solution best = current;

    int bestCost = current.totalCost;

    for (size_t routeAIndex = 0; routeAIndex < current.routes.size(); routeAIndex++) {
        const Route& routeA = current.routes[routeAIndex];

        for (size_t posA = 0; posA < routeA.customers.size(); posA++) {
            int customerA = routeA.customers[posA];
            int demandA = getDemand(instance, customerA);

            for (size_t routeBIndex = routeAIndex; routeBIndex < current.routes.size(); routeBIndex++) {
                const Route& routeB = current.routes[routeBIndex];

                size_t startPosB = 0;

                if (routeAIndex == routeBIndex) {
                    startPosB = posA + 1;
                }

                for (size_t posB = startPosB; posB < routeB.customers.size(); posB++) {
                    int customerB = routeB.customers[posB];
                    int demandB = getDemand(instance, customerB);

                    // Se estiverem em rotas diferentes, precisa verificar capacidade.
                    if (routeAIndex != routeBIndex) {
                        int newLoadA = routeA.load - demandA + demandB;
                        int newLoadB = routeB.load - demandB + demandA;

                        if (newLoadA > instance.capacity || newLoadB > instance.capacity) {
                            continue;
                        }
                    }

                    Solution neighbor = current;

                    std::swap(
                        neighbor.routes[routeAIndex].customers[posA],
                        neighbor.routes[routeBIndex].customers[posB]
                    );

                    updateSolutionInfo(instance, neighbor);

                    if (neighbor.totalCost < bestCost) {
                        best = neighbor;
                        bestCost = neighbor.totalCost;
                    }
                }
            }
        }
    }

    return best;
}

Solution bestTwoOptIntra(
    const Solution& current,
    const CVRPInstance& instance
) {
    Solution best = current;

    int bestCost = current.totalCost;

    for (size_t routeIndex = 0; routeIndex < current.routes.size(); routeIndex++) {
        const Route& route = current.routes[routeIndex];

        if (route.customers.size() < 3) {
            continue;
        }

        for (size_t i = 0; i < route.customers.size() - 1; i++) {
            for (size_t j = i + 1; j < route.customers.size(); j++) {

                Solution neighbor = current;

                std::reverse(
                    neighbor.routes[routeIndex].customers.begin() + i,
                    neighbor.routes[routeIndex].customers.begin() + j + 1
                );

                updateSolutionInfo(instance, neighbor);

                if (neighbor.totalCost < bestCost) {
                    best = neighbor;
                    bestCost = neighbor.totalCost;
                }
            }
        }
    }

    return best;
}

Solution VND(const Solution& initial, const CVRPInstance& instance) {
    Solution current = initial;

    int k = 0;
    int numberOfNeighborhoods = 3;

    while (k < numberOfNeighborhoods) {
        Solution neighbor;

        if (k == 0) {
            neighbor = bestRelocate(current, instance);
        } else if (k == 1) {
            neighbor = bestSwap(current, instance);
        } else if (k == 2) {
            neighbor = bestTwoOptIntra(current, instance);
        }

        if (neighbor.totalCost < current.totalCost) {
            current = neighbor;
            k = 0;
        } else {
            k++;
        }
    }

    return current;
}