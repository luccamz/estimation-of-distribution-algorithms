#include "optimizer.hpp"

class cGA : public Optimizer {
  public:
    cGA(int prob_size, double K_val);
    BenchmarkResult run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) override;
    ~cGA() = default;

  private:
    int n;
    double K;
};
