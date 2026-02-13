#include <individual.hpp>

using FitnessFunction = double (*)(const Individual &);
using TerminationCriterion = bool (*)(int max_evaluations, double best_fitness);

class Algorithm {};
