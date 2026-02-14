#include "sig_cga.hpp"
#include "original_history.hpp"
#include "simplified_history.hpp"

sigcGA::sigcGA(bool simplified_history) {
    simplified_history ? history = new SimplifiedHistory()
                       : history = new OriginalHistory(100); // Assuming n=100
}
BenchmarkResult sigcGA::run(FitnessFunction fitness_func,
                            TerminationCriterion termination_criterion) {
    return BenchmarkResult{0, 0.0}; // Placeholder implementation
}

sigcGA::~sigcGA() { delete history; }
