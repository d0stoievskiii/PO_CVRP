#include "common.hpp"

Solution constructInitialSolution(const CVRPInstance& instance, double alpha, std::mt19937& rng);

Solution graspVND(const CVRPInstance& instance, int maxIterations, double alpha, std::mt19937& rng, const VNDConfig& config);