#include "optimizer.hpp"

class OnePlusOneEA : public Optimizer {
  public:
    OnePlusOneEA(int prob_size) : n(prob_size) {}
    BenchmarkResult run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) override;

  private:
    int n;
};
