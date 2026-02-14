#include "individual.hpp"

Individual Individual::mutate(std::mt19937 &gen) {
    int n = this->bits.size();
    auto child = *this; // Create a copy of the individual to mutate

    // Task 1 Requirement: "Run time... should be linear in the number of flipped bits"
    // Instead of iterating 0..n and flipping with prob 1/n (which is O(n)),
    // we calculate the distance to the *next* flip using a Geometric distribution.
    // This allows us to skip over non-flipped bits in O(1) time.
    double prob = 1.0 / n;
    std::geometric_distribution<int> geo(prob);

    int next_bit = geo(gen);
    while (next_bit < n) {
        child.bits[next_bit] = 1 - this->bits[next_bit];
        next_bit += 1 + geo(gen); // Jump to the next flipped bit
    }
    child.fitness = -1.0; // Invalidadete old fitness
    return child;
}
