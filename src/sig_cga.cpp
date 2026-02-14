#include "sig_cga.hpp"
#include "original_history.hpp"
#include "simplified_history.hpp"

sigcGA::sigcGA(int prob_size, bool simplified_history) {
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
BenchmarkResult sigcGA::run(FitnessFunction fitness_func,
                            TerminationCriterion termination_criterion) {
    return BenchmarkResult{0, 0.0}; // Placeholder implementation
}

sigcGA::~sigcGA() {
    for (History *history : histories) {
        delete history;
    }
}
