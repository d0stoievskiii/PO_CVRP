#include "csv_dumper.hpp"

double calculateGapPercent(int cost, int bks) {
    return 100.0 * static_cast<double>(cost - bks) / static_cast<double>(bks);
}

void writeCsvHeader(std::ofstream& out) {
    out << "instance,set,n_customers,capacity,bks,"
        << "variant,alpha,max_iterations,run,seed,"
        << "cost,gap_percent,time_ms,n_routes,valid\n";
}

void writeCsvRow(std::ofstream& out, const ExperimentResult& r) {
    out << r.instanceName << ","
        << r.instanceSet << ","
        << r.nCustomers << ","
        << r.capacity << ","
        << r.bks << ","
        << r.variant << ","
        << std::fixed << std::setprecision(2) << r.alpha << ","
        << r.maxIterations << ","
        << r.run << ","
        << r.seed << ","
        << r.cost << ","
        << std::fixed << std::setprecision(4) << r.gapPercent << ","
        << r.timeMs << ","
        << r.nRoutes << ","
        << (r.valid ? 1 : 0)
        << "\n";
}