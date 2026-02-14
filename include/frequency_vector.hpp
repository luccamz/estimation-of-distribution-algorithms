#include "individual.hpp"
#include <random>
#include <vector>

struct FrequencyVector {
    // Instantiate a standard frequency vector of size n
    // with all freqs set to 1/2
    FrequencyVector(int n_size) : n(n_size), p(n_size, 0.5) {}

    // Updates frequency at index i and restricts to [1/n, 1 - 1/n] as per
    // Equation 2
    void update_frequency(int i, double freq) {
        double lower = 1.0 / n;
        double upper = 1.0 - lower;
        p[i] = std::max(std::min(freq, upper), lower);
    }

    // Samples an individual based on independent probabilities
    Individual sample(std::mt19937 &gen) {
        Individual ind(n);
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        for (int i = 0; i < n; ++i) {
            ind.bits[i] = (dist(gen) < p[i]) ? 1 : 0;
        }
        return ind;
    }

  private:
    std::vector<double> p;
    int n;
};
