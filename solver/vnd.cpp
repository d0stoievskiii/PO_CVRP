#include "vnd.hpp"

Solution bestRelocate(
    const Solution& current,
    const CVRPInstance& instance
) {
    int bestDelta = 0;

    size_t bestOriginRoute = 0;
    size_t bestOriginPos = 0;
    size_t bestDestRoute = 0;
    size_t bestDestPos = 0;

    bool foundImprovement = false;

    for (size_t originRouteIndex = 0; originRouteIndex < current.routes.size(); originRouteIndex++) {
        const Route& originRoute = current.routes[originRouteIndex];

        for (size_t originPos = 0; originPos < originRoute.customers.size(); originPos++) {
            int customer = originRoute.customers[originPos];
            int customerDemand = getDemand(instance, customer);

            for (size_t destRouteIndex = 0; destRouteIndex < current.routes.size(); destRouteIndex++) {
                const Route& destRoute = current.routes[destRouteIndex];

                for (size_t destPos = 0; destPos <= destRoute.customers.size(); destPos++) {
                    if (originRouteIndex == destRouteIndex) {
                        if (destPos == originPos || destPos == originPos + 1) {
                            continue;
                        }
                    }

                    if (originRouteIndex != destRouteIndex) {
                        if (destRoute.load + customerDemand > instance.capacity) {
                            continue;
                        }
                    }

                    int oldCost;
                    int newCost;

                    if (originRouteIndex == destRouteIndex) {
                        std::vector<int> candidate = originRoute.customers;

                        candidate.erase(candidate.begin() + originPos);

                        size_t adjustedDestPos = destPos;
                        if (destPos > originPos) {
                            adjustedDestPos--;
                        }

                        if (adjustedDestPos > candidate.size()) {
                            continue;
                        }

                        candidate.insert(candidate.begin() + adjustedDestPos, customer);

                        oldCost = originRoute.cost;
                        newCost = routeCostWithCustomers(instance, candidate);
                    } else {
                        std::vector<int> candidateOrigin = originRoute.customers;
                        std::vector<int> candidateDest = destRoute.customers;

                        candidateOrigin.erase(candidateOrigin.begin() + originPos);
                        candidateDest.insert(candidateDest.begin() + destPos, customer);

                        oldCost = originRoute.cost + destRoute.cost;
                        newCost =
                            routeCostWithCustomers(instance, candidateOrigin)
                            + routeCostWithCustomers(instance, candidateDest);
                    }

                    int delta = newCost - oldCost;

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestOriginRoute = originRouteIndex;
                        bestOriginPos = originPos;
                        bestDestRoute = destRouteIndex;
                        bestDestPos = destPos;
                        foundImprovement = true;
                    }
                }
            }
        }
    }

    if (!foundImprovement) {
        return current;
    }

    Solution result = current;

    int customer = result.routes[bestOriginRoute].customers[bestOriginPos];

    result.routes[bestOriginRoute].customers.erase(
        result.routes[bestOriginRoute].customers.begin() + bestOriginPos
    );

    size_t adjustedDestPos = bestDestPos;

    if (bestOriginRoute == bestDestRoute && bestDestPos > bestOriginPos) {
        adjustedDestPos--;
    }

    result.routes[bestDestRoute].customers.insert(
        result.routes[bestDestRoute].customers.begin() + adjustedDestPos,
        customer
    );

    updateRouteInfo(instance, result.routes[bestOriginRoute]);

    if (bestOriginRoute != bestDestRoute) {
        updateRouteInfo(instance, result.routes[bestDestRoute]);
    }

    removeEmptyRoutes(result);

    result.totalCost = current.totalCost + bestDelta;

    // Se removeu rota vazia, melhor recalcular só uma vez por segurança.
    updateSolutionInfo(instance, result);

    return result;
}

Solution bestSwap(
    const Solution& current,
    const CVRPInstance& instance
) {
    int bestDelta = 0;

    size_t bestRouteA = 0;
    size_t bestPosA = 0;
    size_t bestRouteB = 0;
    size_t bestPosB = 0;

    bool foundImprovement = false;

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

                    if (routeAIndex != routeBIndex) {
                        int newLoadA = routeA.load - demandA + demandB;
                        int newLoadB = routeB.load - demandB + demandA;

                        if (newLoadA > instance.capacity || newLoadB > instance.capacity) {
                            continue;
                        }
                    }

                    int oldCost;
                    int newCost;

                    if (routeAIndex == routeBIndex) {
                        std::vector<int> candidate = routeA.customers;
                        std::swap(candidate[posA], candidate[posB]);

                        oldCost = routeA.cost;
                        newCost = routeCostWithCustomers(instance, candidate);
                    } else {
                        std::vector<int> candidateA = routeA.customers;
                        std::vector<int> candidateB = routeB.customers;

                        std::swap(candidateA[posA], candidateB[posB]);

                        oldCost = routeA.cost + routeB.cost;
                        newCost =
                            routeCostWithCustomers(instance, candidateA)
                            + routeCostWithCustomers(instance, candidateB);
                    }

                    int delta = newCost - oldCost;

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestRouteA = routeAIndex;
                        bestPosA = posA;
                        bestRouteB = routeBIndex;
                        bestPosB = posB;
                        foundImprovement = true;
                    }
                }
            }
        }
    }

    if (!foundImprovement) {
        return current;
    }

    Solution result = current;

    std::swap(
        result.routes[bestRouteA].customers[bestPosA],
        result.routes[bestRouteB].customers[bestPosB]
    );

    updateRouteInfo(instance, result.routes[bestRouteA]);

    if (bestRouteA != bestRouteB) {
        updateRouteInfo(instance, result.routes[bestRouteB]);
    }

    result.totalCost = current.totalCost + bestDelta;

    return result;
}

Solution bestTwoOptIntra(
    const Solution& current,
    const CVRPInstance& instance
) {
    int bestDelta = 0;
    size_t bestRouteIndex = 0;
    size_t bestI = 0;
    size_t bestJ = 0;
    bool foundImprovement = false;

    for (size_t routeIndex = 0; routeIndex < current.routes.size(); routeIndex++) {
        const Route& route = current.routes[routeIndex];

        if (route.customers.size() < 3) {
            continue;
        }

        int oldRouteCost = route.cost;

        for (size_t i = 0; i < route.customers.size() - 1; i++) {
            for (size_t j = i + 1; j < route.customers.size(); j++) {
                std::vector<int> candidateCustomers = route.customers;

                std::reverse(
                    candidateCustomers.begin() + i,
                    candidateCustomers.begin() + j + 1
                );

                int newRouteCost = routeCostWithCustomers(instance, candidateCustomers);
                int delta = newRouteCost - oldRouteCost;

                if (delta < bestDelta) {
                    bestDelta = delta;
                    bestRouteIndex = routeIndex;
                    bestI = i;
                    bestJ = j;
                    foundImprovement = true;
                }
            }
        }
    }

    if (!foundImprovement) {
        return current;
    }

    Solution result = current;

    std::reverse(
        result.routes[bestRouteIndex].customers.begin() + bestI,
        result.routes[bestRouteIndex].customers.begin() + bestJ + 1
    );

    updateRouteInfo(instance, result.routes[bestRouteIndex]);
    result.totalCost = current.totalCost + bestDelta;

    return result;
}


Solution bestTwoOptInter(
    const Solution& current,
    const CVRPInstance& instance
) {
    int bestDelta = 0;

    size_t bestRouteA = 0;
    size_t bestRouteB = 0;
    size_t bestCutA = 0;
    size_t bestCutB = 0;

    bool foundImprovement = false;

    for (size_t routeAIndex = 0; routeAIndex < current.routes.size(); routeAIndex++) {
        const Route& routeA = current.routes[routeAIndex];

        for (size_t routeBIndex = routeAIndex + 1; routeBIndex < current.routes.size(); routeBIndex++) {
            const Route& routeB = current.routes[routeBIndex];

            /*
                cutA representa quantos clientes ficam no prefixo da rota A.

                Exemplo:
                    routeA = [10, 20, 30, 40]

                cutA = 0:
                    prefixA = []
                    suffixA = [10, 20, 30, 40]

                cutA = 2:
                    prefixA = [10, 20]
                    suffixA = [30, 40]

                cutA = 4:
                    prefixA = [10, 20, 30, 40]
                    suffixA = []

                Permitimos 0 e size(), mas depois evitamos gerar rotas vazias
                se isso for indesejado.
            */

            for (size_t cutA = 0; cutA <= routeA.customers.size(); cutA++) {
                for (size_t cutB = 0; cutB <= routeB.customers.size(); cutB++) {

                    std::vector<int> newRouteA;
                    std::vector<int> newRouteB;

                    // newRouteA = prefixA + suffixB
                    newRouteA.insert(
                        newRouteA.end(),
                        routeA.customers.begin(),
                        routeA.customers.begin() + cutA
                    );

                    newRouteA.insert(
                        newRouteA.end(),
                        routeB.customers.begin() + cutB,
                        routeB.customers.end()
                    );

                    // newRouteB = prefixB + suffixA
                    newRouteB.insert(
                        newRouteB.end(),
                        routeB.customers.begin(),
                        routeB.customers.begin() + cutB
                    );

                    newRouteB.insert(
                        newRouteB.end(),
                        routeA.customers.begin() + cutA,
                        routeA.customers.end()
                    );

                    // Evita rotas vazias. Pode remover esta restrição se quiser
                    // permitir que uma rota desapareça.
                    if (newRouteA.empty() || newRouteB.empty()) {
                        continue;
                    }

                    int loadA = calculateRouteLoad(instance, newRouteA);
                    int loadB = calculateRouteLoad(instance, newRouteB);

                    if (loadA > instance.capacity || loadB > instance.capacity) {
                        continue;
                    }

                    int oldCost = routeA.cost + routeB.cost;

                    int newCost =
                        routeCostWithCustomers(instance, newRouteA)
                        + routeCostWithCustomers(instance, newRouteB);

                    int delta = newCost - oldCost;

                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestRouteA = routeAIndex;
                        bestRouteB = routeBIndex;
                        bestCutA = cutA;
                        bestCutB = cutB;
                        foundImprovement = true;
                    }
                }
            }
        }
    }

    if (!foundImprovement) {
        return current;
    }

    Solution result = current;

    std::vector<int> oldA = current.routes[bestRouteA].customers;
    std::vector<int> oldB = current.routes[bestRouteB].customers;

    std::vector<int> newA;
    std::vector<int> newB;

    newA.insert(
        newA.end(),
        oldA.begin(),
        oldA.begin() + bestCutA
    );

    newA.insert(
        newA.end(),
        oldB.begin() + bestCutB,
        oldB.end()
    );

    newB.insert(
        newB.end(),
        oldB.begin(),
        oldB.begin() + bestCutB
    );

    newB.insert(
        newB.end(),
        oldA.begin() + bestCutA,
        oldA.end()
    );

    result.routes[bestRouteA].customers = newA;
    result.routes[bestRouteB].customers = newB;

    updateSolutionInfo(instance, result);

    if (!validateSolution(instance, result)) {
    std::cerr << "Erro gerado em bestTwoOptInter\n";
    std::cerr << "bestRouteA=" << bestRouteA
              << " bestCutA=" << bestCutA
              << " bestRouteB=" << bestRouteB
              << " bestCutB=" << bestCutB
              << "\n";

    std::cout << current;
    std::cout << result;

    throw std::runtime_error("bestTwoOptInter gerou solucao invalida.");
}

    return result;
}

Solution VND(const Solution& initial, const CVRPInstance& instance, const VNDConfig& config) {
    Solution current = initial;

    std::vector<int> neighborhoods;

    if (config.useRelocate) neighborhoods.push_back(0);
    if (config.useSwap) neighborhoods.push_back(1);
    if (config.useTwoOpt) neighborhoods.push_back(2);
    if (config.useTwoOptInter) neighborhoods.push_back(3);

    if (neighborhoods.empty()) {
        return current;
    }

    int k = 0;

    while (k < static_cast<int>(neighborhoods.size())) {
        Solution neighbor;

        if (k == 0) {
            neighbor = bestRelocate(current, instance);
        } else if (k == 1) {
            neighbor = bestSwap(current, instance);
        } else if (k == 2) {
            neighbor = bestTwoOptIntra(current, instance);
        }  else if (k == 3) {
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