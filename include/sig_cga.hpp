#include "history.hpp"
#include "optimizer.hpp"
#include <cstdint>

class sigcGA : public Optimizer {
  public:
    sigcGA(int prob_size, double eps, bool simplified_history = true);
    BenchmarkResult run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) override;
    ~sigcGA() override;

  private:
    double eps;
    double ln_n;
    std::vector<History *> histories;
    enum class Decision : uint8_t { UP, DOWN, STAY };
    Decision sig(double p, HistoryTriple &H);
};
