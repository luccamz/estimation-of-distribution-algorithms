#include "one_plus_one_ea.hpp"
#include "individual.hpp"

BenchmarkResult OnePlusOneEA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {

    // Algorithm 4, (1+1) EA
    auto p = FrequencyVector(n);
    auto xt = p.sample(gen); // individual chosen uniformly at random

    *xt.fitness = f(xt);
    int fitness_evals = 1;
    while (!tc(fitness_evals, *xt.fitness)) {
        auto yt = xt.mutate(gen); // apply standard bit mutation
        xt = f(yt) >= *xt.fitness ? yt : xt;
        fitness_evals++;
    }
    return BenchmarkResult{fitness_evals, f(xt)};
}
