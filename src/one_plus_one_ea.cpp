#include "one_plus_one_ea.hpp"
#include "individual.hpp"

BenchmarkResult OnePlusOneEA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {

    // Algorithm 4, (1+1) EA
    int t = 0;
    auto p = FrequencyVector(n);
    auto xt = p.sample(gen); // individual chosen uniformly at random
    while (!tc(t, f(xt))) {
        auto yt = xt.mutate(gen); // apply standard bit mutation
        xt = f(yt) >= f(xt) ? yt : xt;
        t++;
    }
    return BenchmarkResult{t, f(xt)};
}
