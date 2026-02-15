#include "sig_cga.hpp"
#include "original_history.hpp"
#include "simplified_history.hpp"
#include <algorithm>
#include <cmath>

sigcGA::sigcGA(int prob_size, double eps_val, bool simplified_history) {
    eps = eps_val;
    ln_n = std::log(prob_size);
    histories.reserve(prob_size);
    if (simplified_history) {
        for (int i = 0; i < prob_size; ++i) {
            histories.push_back(new SimplifiedHistory());
        }
    } else {
        for (int i = 0; i < prob_size; ++i) {
            histories.push_back(new OriginalHistory(prob_size));
        }
    }
}

sigcGA::Decision sigcGA::sig(double p, HistoryTriple &H) {
    // Implements Equation (3) from the pdf.
    // We compare the number of observed 1s (or 0s) against the expected mean
    // plus a deviation term based on the Chernoff bound.

    auto [h1, h0, m] = H;

    // Case 1: Frequency is low (<= 0.5), check for significant increase (UP)
    // Deviation term: epsilon * max(sqrt(variance * ln n), ln n)
    if (p <= 0.5 && h1 >= m * p + eps * std::max(std::sqrt(m * p * ln_n), ln_n))
        return Decision::UP;

    // Case 2: Frequency is high (>= 0.5), check for significant decrease (DOWN)
    else if (p >= 0.5 && h0 >= m * (1 - p) + eps * std::max(std::sqrt(m * (1 - p) * ln_n), ln_n))
        return Decision::DOWN;
    return Decision::STAY;
}

BenchmarkResult sigcGA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {
    // Algorithm 1
    int fitness_evals = 0;
    int n = histories.size();
    auto pt = FrequencyVector(n);
    // empty the histories
    std::for_each(histories.begin(), histories.end(), [](History *h) { h->wipe(); });
    auto xt1 = Individual(n);
    auto xt2 = Individual(n);
    auto curr_fitness = 0.0;
    while (!tc(fitness_evals, curr_fitness)) {
        pt.sample(gen, xt1);
        pt.sample(gen, xt2);
        xt1.fitness = f(xt1);
        xt2.fitness = f(xt2);
        fitness_evals += 2; // properly count number of f calls
        if (xt1.fitness < xt2.fitness)
            xt1 = xt2;

        int i = 0;
        while (i < n) {
            histories[i]->add(xt1.bits[i]);

            // Check subsequences for significance
            for (HistoryTriple &h : histories[i]->get_subsequences()) {
                auto s = sig(pt.p[i], h);

                if (s == Decision::STAY)
                    continue;

                if (s == Decision::UP)
                    pt.update_frequency(i, 1 - 1.0 / n);

                else if (s == Decision::DOWN) // to be explicit
                    pt.update_frequency(i, 1.0 / n);

                // Reset history after a significant update
                histories[i]->wipe();
                break;
            }
            i++;
        }

        curr_fitness = xt1.fitness;
    }
    return BenchmarkResult{fitness_evals, curr_fitness};
}

sigcGA::~sigcGA() {
    for (History *history : histories) {
        delete history;
    }
}
