#include <chrono>


#include "parse_instance/dotvrp.hpp"
#include "helpers.hpp"
#include "solver/grasp.hpp"
#include "solver/vnd.hpp"
#include "csv/csv_dumper.hpp"

struct InstanceFile {
    std::string name;
    std::string set;
    std::string vrpPath;
    std::string solPath;
};

void runExperiments() {
    std::vector<InstanceFile> instances = {
        {"A-n32-k5", "A", "instances/A/A-n32-k5.vrp", "instances/A/A-n32-k5.sol"},
        {"A-n33-k5", "A", "instances/A/A-n33-k5.vrp", "instances/A/A-n33-k5.sol"},
        {"A-n37-k6", "A", "instances/A/A-n37-k6.vrp", "instances/A/A-n37-k6.sol"},

        {"B-n31-k5", "B", "instances/B/B-n31-k5.vrp", "instances/B/B-n31-k5.sol"},
        {"B-n34-k5", "B", "instances/B/B-n34-k5.vrp", "instances/B/B-n34-k5.sol"},

        {"P-n16-k8", "P", "instances/P/P-n16-k8.vrp", "instances/P/P-n16-k8.sol"},
        {"P-n19-k2", "P", "instances/P/P-n19-k2.vrp", "instances/P/P-n19-k2.sol"},

        {"E-n22-k4", "E", "instances/E/E-n22-k4.vrp", "instances/E/E-n22-k4.sol"},
        {"E-n51-k5", "E", "instances/E/E-n51-k5.vrp", "instances/E/E-n51-k5.sol"}
    };

    std::vector<AlgorithmVariant> variants = {
        {"G0", {false, false, false}},
        {"G1", {true,  false, false}},
        {"G2", {true,  true,  false}},
        {"G3", {true,  true,  true }}
    };

    double alpha = 0.3;
    int maxIterations = 100;
    int runsPerInstance = 10;

    std::ofstream out("results.csv");
    writeCsvHeader(out);

    for (const InstanceFile& file : instances) {
        std::cout << "Instancia: " << file.name << "\n";

        CVRPInstance instance = readVRP(file.vrpPath);
        instance.distance = buildDistanceMatrix(instance);

        Solution reference = readSolution(file.solPath);
        int bks = reference.totalCost;

        for (const AlgorithmVariant& variant : variants) {
            std::cout << "  Variante: " << variant.name << "\n";

            for (int run = 1; run <= runsPerInstance; run++) {
                unsigned int seed = 100000
                    + static_cast<unsigned int>(run)
                    + static_cast<unsigned int>(variant.name[1]) * 100
                    + static_cast<unsigned int>(instance.dimension) * 1000;

                std::mt19937 rng(seed);

                auto start = std::chrono::high_resolution_clock::now();

                Solution best = graspVND(
                    instance,
                    maxIterations,
                    alpha,
                    rng,
                    variant.config
                );

                auto end = std::chrono::high_resolution_clock::now();

                updateSolutionInfo(instance, best);
                bool valid = validateSolution(instance, best);

                long long timeMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start
                    ).count();

                ExperimentResult result;
                result.instanceName = file.name;
                result.instanceSet = file.set;
                result.nCustomers = instance.dimension - 1;
                result.capacity = instance.capacity;
                result.bks = bks;
                result.variant = variant.name;
                result.alpha = alpha;
                result.maxIterations = maxIterations;
                result.run = run;
                result.seed = seed;
                result.cost = best.totalCost;
                result.gapPercent = calculateGapPercent(best.totalCost, bks);
                result.timeMs = timeMs;
                result.nRoutes = static_cast<int>(best.routes.size());
                result.valid = valid;

                writeCsvRow(out, result);

                std::cout << "    run " << run
                          << " cost=" << best.totalCost
                          << " gap=" << result.gapPercent
                          << "% time=" << timeMs
                          << "ms valid=" << valid
                          << "\n";
            }
        }
    }
}


int main() {
    try {
        runExperiments();
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
    }

    return 0;
}