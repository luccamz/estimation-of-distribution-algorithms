#include "cga.hpp"

cGA::cGA(int prob_size, double K_val) : n(prob_size), K(K_val) {}

BenchmarkResult cGA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {
    // Algorithm 3
    int t = 0;
    auto pt = FrequencyVector(n);
    auto xt1 = Individual(n);
    auto curr_fitness = f(xt1);
    while (!tc(t, curr_fitness)) {
        xt1 = pt.sample(gen);
        auto xt2 = pt.sample(gen);

        if (f(xt1) < f(xt2))
            std::swap(xt1, xt2);
        int i = 0;
        while (i < n) {
            pt.update_frequency(i, pt.p[i] + (xt1.bits[i] - xt2.bits[i]) / K);
            i++;
        }
        curr_fitness = f(xt1);
        t++;
    }
    return BenchmarkResult{t, curr_fitness};
}
