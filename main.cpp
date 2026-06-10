
#include "parse_instance/dotvrp.hpp"
#include "helpers.hpp"
#include "solver/grasp.hpp"
#include "solver/vnd.hpp"


int main() {
    try {
        CVRPInstance instance = readVRP("instances/A-n32-k5.vrp");
        Solution sol = readSolution("instances/A-n32-k5.sol");

        auto dist = buildDistanceMatrix(instance);
        instance.distance = dist;

        

        
        std::cout << "Instancia: " << instance.name << "\n";
        std::cout << "Tipo: " << instance.type << "\n";
        std::cout << "Dimensao: " << instance.dimension << "\n";
        std::cout << "Capacidade: " << instance.capacity << "\n";
        std::cout << "Deposito: " << instance.depotId << "\n";
        
        std::cout << "Custo informado no .sol: " << sol.totalCost << "\n";

        
        Solution s = graspVND(instance, 50, 0.5);
        int computedCost = solutionCost(s, dist, instance.depotId);
        std::cout << "Custo calculado: " << computedCost << "\n";


    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
    }

    return 0;
}