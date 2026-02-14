#include "individual.hpp"

inline double one_max(const Individual &ind) { return std::accumulate(ind.bits.begin(), ind.bits.end(), 0); }

inline double leading_ones(const Individual &ind) {
    int i = 0;
    while (i < ind.bits.size() && ind.bits[i] == 1) {
        i++;
    }
    return i;
}

inline double jump(const Individual &ind, int k = 3) {
    int n = ind.bits.size();
    int ones_count = std::accumulate(ind.bits.begin(), ind.bits.end(), 0);
    if (ones_count <= n - k) {
        return ones_count; // Reward for having more ones
    } else if (ones_count == n) {
        return n; // Optimal solution
    } else {
        return n - ones_count; // Penalize for being in the gap
    }
}

inline bool optimal_or_max_evals(int curr_evaluations, double fitness, int max_evals = 1000, double opt_fitness = 100) {
    return curr_evaluations >= max_evals || fitness >= opt_fitness;
}
