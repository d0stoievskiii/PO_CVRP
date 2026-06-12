#include <fstream>
#include <iomanip>

struct ExperimentResult {
    std::string instanceName;
    std::string instanceSet;
    int nCustomers;
    int capacity;
    int bks;

    std::string variant;
    double alpha;
    int maxIterations;
    int run;
    unsigned int seed;

    int cost;
    double gapPercent;
    long long timeMs;
    int nRoutes;
    bool valid;
};

double calculateGapPercent(int cost, int bks);

void writeCsvHeader(std::ofstream& out);

void writeCsvRow(std::ofstream& out, const ExperimentResult& r);