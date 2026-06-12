#include "grasp.hpp"
#include "vnd.hpp"


Solution constructInitialSolution(
    const CVRPInstance& instance,
    double alpha,
    std::mt19937& rng
) {
    if (alpha < 0.0 || alpha > 1.0) {
        throw std::invalid_argument("alpha deve estar entre 0.0 e 1.0");
    }

    Solution solution;

    int depotId = instance.depotId;

    std::vector<int> unrouted;

    for (int id = 1; id <= instance.dimension; id++) {
        if (id == depotId) {
            continue;
        }

        int demand = getDemand(instance, id);

        if (demand > instance.capacity) {
            throw std::runtime_error(
                "Cliente " + std::to_string(id) +
                " possui demanda " + std::to_string(demand) +
                " maior que a capacidade " +
                std::to_string(instance.capacity)
            );
        }

        unrouted.push_back(id);
    }

    while (!unrouted.empty()) {
        Route route;
        int currentNode = depotId;

        bool insertedAtLeastOne = false;

        while (true) {
            std::vector<Candidate> candidates;

            for (int customerId : unrouted) {
                int demand = getDemand(instance, customerId);

                if (route.load + demand <= instance.capacity) {
                    int insertionCost =
                        dist(instance, currentNode, customerId)
                        + dist(instance, customerId, depotId)
                        - dist(instance, currentNode, depotId);

                    candidates.push_back({customerId, insertionCost});
                }
            }

            if (candidates.empty()) {
                break;
            }

            int minCost = std::numeric_limits<int>::max();
            int maxCost = std::numeric_limits<int>::min();

            for (const Candidate& c : candidates) {
                minCost = std::min(minCost, c.insertionCost);
                maxCost = std::max(maxCost, c.insertionCost);
            }

            double limit = minCost + alpha * (maxCost - minCost);

            std::vector<Candidate> rcl;

            for (const Candidate& c : candidates) {
                if (c.insertionCost <= limit) {
                    rcl.push_back(c);
                }
            }

            std::uniform_int_distribution<int> distribution(
                0,
                static_cast<int>(rcl.size()) - 1
            );

            Candidate chosen = rcl[distribution(rng)];
            int chosenDemand = getDemand(instance, chosen.customerId);

            // Trava de segurança.
            if (route.load + chosenDemand > instance.capacity) {
                std::cerr << "route.load=" << route.load
                    << " chosenDemand=" << chosenDemand
                    << " capacity=" << instance.capacity
                    << "\n";

                throw std::runtime_error("BUG: insercao excederia capacidade.");
            }

            route.customers.push_back(chosen.customerId);
            route.load += getDemand(instance, chosen.customerId);
            currentNode = chosen.customerId;

            unrouted.erase(
                std::remove(unrouted.begin(), unrouted.end(), chosen.customerId),
                unrouted.end()
            );

            insertedAtLeastOne = true;
        }

        if (!insertedAtLeastOne) {
            std::cerr << "Clientes restantes:\n";
            for (int customerId : unrouted) {
                std::cerr << "Cliente " << customerId
                          << " demanda=" << getDemand(instance, customerId)
                          << "\n";
            }

            throw std::runtime_error(
                "Nao foi possivel inserir nenhum cliente. "
                "Verifique demandas, capacidade e IDs dos clientes."
            );
        }

        if (route.customers.empty()) {
            throw std::runtime_error("Rota vazia criada.");
        }

        updateRouteInfo(instance, route);

        if (route.load > instance.capacity) {
            throw std::runtime_error("BUG: rota construida excede capacidade.");
        }
        solution.routes.push_back(route);
    }

    solution.totalCost = calculateSolutionCost(instance, solution);
    updateSolutionInfo(instance, solution);

    return solution;
}

Solution graspVND(
    const CVRPInstance& instance,
    int maxIterations,
    double alpha
) {
    Solution bestSolution;
    bestSolution.totalCost = std::numeric_limits<int>::max();

    std::random_device rd;
    std::mt19937 rng(rd());

    for (int it = 0; it < maxIterations; it++) {
        Solution s = constructInitialSolution(instance, alpha, rng);
        updateSolutionInfo(instance, s);

        s = VND(s, instance);

        if (s.totalCost < bestSolution.totalCost) {
            bestSolution = s;
        }
    }

    return bestSolution;
}