#include "cga.hpp"
#include <algorithm>

cGA::cGA(int prob_size, double K_val) : n(prob_size), K(K_val) {}

BenchmarkResult cGA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {
    // Algorithm 3
    int fitness_evals = 0;
    auto pt = FrequencyVector(n);
    auto xt1 = Individual(n);
    auto curr_fitness = f(xt1);
    while (!tc(fitness_evals, curr_fitness)) {
        xt1 = pt.sample(gen);
        auto xt2 = pt.sample(gen);

        xt1.fitness = f(xt1);
        xt2.fitness = f(xt2);
        fitness_evals += 2; // properly count fitness evaluations
        if (xt1.fitness < xt2.fitness)
            std::swap(xt1, xt2);
        int i = 0;
        while (i < n) {
            pt.update_frequency(i, pt.p[i] + (static_cast<double>(xt1.bits[i]) - static_cast<double>(xt2.bits[i])) / K);
            i++;
        }
        curr_fitness = xt1.fitness;
    }
    return BenchmarkResult{fitness_evals, curr_fitness};
}
