#include "individual.hpp"
#include <algorithm>
#include <random>
#include <vector>

class FrequencyVector {
  public:
    std::vector<double> p;
    int n;

    FrequencyVector(int n_size) : n(n_size), p(n_size, 0.5) {}

    // Restricts frequencies to [1/n, 1 - 1/n] as per Equation 2
    void restrict_frequency(int i) {
        double lower = 1.0 / n;
        double upper = 1.0 - lower;
        p[i] = std::max(std::min(p[i], upper), lower);
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
};
