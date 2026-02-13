#include "individual.hpp"

void mutate(Individual &ind, std::mt19937 &gen) {
    int n = ind.bits.size();
    if (n == 0)
        return;

    double prob = 1.0 / n; // Standard bit mutation probability
    std::geometric_distribution<int> geo(prob);

    int next_bit = geo(gen);
    while (next_bit < n) {
        ind.bits[next_bit] = 1 - ind.bits[next_bit];
        next_bit += 1 + geo(gen); // O(1) jump logic
    }
}

double one_max(const Individual &ind) {
    return std::accumulate(ind.bits.begin(), ind.bits.end(), 0);
}

double leading_ones(const Individual &ind) {
    int i = 0;
    while (i < ind.bits.size() && ind.bits[i] == 1) {
        i++;
    }
    return i;
}

double jump(const Individual &ind, int k = 3) {
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
