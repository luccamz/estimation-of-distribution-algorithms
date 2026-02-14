#include "individual.hpp"

Individual Individual::mutate(Individual &ind, std::mt19937 &gen) {
    int n = ind.bits.size();
    auto child = copy(); // Create a copy of the individual to mutate

    double prob = 1.0 / n; // Standard bit mutation probability
    std::geometric_distribution<int> geo(prob);

    int next_bit = geo(gen);
    while (next_bit < n) {
        child.bits[next_bit] = 1 - ind.bits[next_bit];
        next_bit += 1 + geo(gen); // O(1) jump logic
    }
    return child;
}
