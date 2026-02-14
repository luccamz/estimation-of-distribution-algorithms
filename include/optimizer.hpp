#include "individual.hpp"

using FitnessFunction = double (*)(const Individual &);
using TerminationCriterion = bool (*)(int curr_evaluations, double fitness);
struct BenchmarkResult {
    int evaluations;
    double best_fitness;
};

class Optimizer {
  public:
    virtual ~Optimizer() = default;
    virtual BenchmarkResult run(FitnessFunction fitness_func,
                                TerminationCriterion termination_criterion) = 0;
};
