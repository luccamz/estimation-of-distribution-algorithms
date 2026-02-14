#pragma once
#include "frequency_vector.hpp"
#include "individual.hpp"
#include <functional>

using FitnessFunction = double (*)(const Individual &);
using TerminationCriterion = std::function<bool(int, double)>;
struct BenchmarkResult {
    int evaluations;
    double best_fitness;
};

class Optimizer {
  public:
    virtual ~Optimizer() = default;
    virtual BenchmarkResult run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) = 0;
};
