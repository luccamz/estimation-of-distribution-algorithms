#include "history.hpp"
#include "optimizer.hpp"

class sigcGA : public Optimizer {
  public:
    sigcGA(int prob_size, bool simplified_history = true);
    BenchmarkResult run(FitnessFunction fitness_func,
                        TerminationCriterion termination_criterion) override;
    ~sigcGA() override;

  private:
    std::vector<History *> histories;
};
