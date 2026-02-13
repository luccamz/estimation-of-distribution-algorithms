#include "individual.hpp"

using FitnessFunction = double (*)(const Individual &);
using TerminationCriterion = bool (*)(int curr_evaluations, double fitness);

class Algorithm {
  public:
    void run(FitnessFunction fitness_func,
             TerminationCriterion termination_criterion);
};
