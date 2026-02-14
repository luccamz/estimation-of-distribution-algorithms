#include "sig_cga.hpp"
#include "original_history.hpp"
#include "simplified_history.hpp"
#include <algorithm>

sigcGA::sigcGA(int prob_size, double eps_val, bool simplified_history) {
    eps = eps_val;
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
    // As per equation (3)
    auto ln_n = std::log(histories.size());
    auto [m, h0, h1] = H;
    if (p <= 0.5 && h1 >= m * p + eps * std::max(std::sqrt(m * p * ln_n), ln_n))
        return Decision::UP;
    else if (p >= 0.5 && h0 >= m * p + eps * std::max(std::sqrt(m * p * ln_n), ln_n))
        return Decision::DOWN;
    return Decision::STAY;
}

BenchmarkResult sigcGA::run(FitnessFunction f, TerminationCriterion tc, std::mt19937 &gen) {
    // Algorithm 1
    int t = 0;
    int n = histories.size();
    auto pt = FrequencyVector(n);
    // empty the histories
    std::for_each(histories.begin(), histories.end(), [](History *h) { h->wipe(); });
    auto xt1 = Individual(n);
    auto curr_fitness = f(xt1);
    while (!tc(t, curr_fitness)) {
        xt1 = pt.sample(gen);
        auto xt2 = pt.sample(gen);

        if (f(xt1) < f(xt2))
            // std::swap(xt1, xt2);
            xt1 = xt2.copy();

        int i = 0;
        while (i < n) {
            histories[i]->add(xt1.bits[i]);
            for (HistoryTriple &h : histories[i]->get_subsequences()) {
                auto s = sig(pt.p[i], h);

                if (s == Decision::STAY)
                    continue;

                if (s == Decision::UP)
                    pt.update_frequency(i, 1 - 1.0 / n);
                else if (s == Decision::DOWN) // to be explicit
                    pt.update_frequency(i, 1.0 / n);
                histories[i]->wipe();
                break;
            }
            i++;
        }

        curr_fitness = f(xt1);
        t++;
    }
    return BenchmarkResult{0, 0.0}; // Placeholder implementation
}

sigcGA::~sigcGA() {
    for (History *history : histories) {
        delete history;
    }
}
