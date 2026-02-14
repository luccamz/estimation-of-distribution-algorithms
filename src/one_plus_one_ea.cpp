#include "one_plus_one_ea.hpp"
#include "individual.hpp"

BenchmarkResult OnePlusOneEA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {

    // Algorithm 4, (1+1) EA
    auto p = FrequencyVector(n);
    auto xt = p.sample(gen); // individual chosen uniformly at random

    xt.fitness = f(xt);
    int fitness_evals = 1;
    while (!tc(fitness_evals, xt.fitness)) {
        auto yt = xt.mutate(gen); // apply standard bit mutation
        yt.fitness = f(yt);
        fitness_evals++;

        xt = yt.fitness >= xt.fitness ? yt : xt;
    }
    return BenchmarkResult{fitness_evals, xt.fitness};
}
