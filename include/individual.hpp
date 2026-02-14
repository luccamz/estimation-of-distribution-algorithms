#include <cstdint>
#include <random>
#include <vector>

struct Individual {
    std::vector<uint8_t> bits;

    Individual(int n) : bits(n, 0) {};

    // copies individual to a new one, useful for mutation
    Individual copy() const {
        Individual ind(bits.size());
        ind.bits = bits;
        return ind;
    }

    // implements standard bit mutation with O(1) jumps
    Individual mutate(Individual &ind, std::mt19937 &gen);
};
