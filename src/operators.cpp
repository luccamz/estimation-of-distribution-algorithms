#include "individual.hpp"

Individual::Individual(int n) : bits(n, 0) {}

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
