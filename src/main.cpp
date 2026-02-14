#include <iostream>
#include <random>
#include <string>

#include "benchmarks.hpp"
#include "cga.hpp"
#include "one_plus_one_ea.hpp"
#include "optimizer.hpp"
#include "sig_cga.hpp"

// --- Helper Functions ---
double jump_k2(const Individual &ind) { return jump(ind, 2); }
double jump_k3(const Individual &ind) { return jump(ind, 3); }

// --- Main Argument Parser ---
int main(int argc, char *argv[]) {
    // Usage: ./algo_exec <Algo> <Problem> <N> <Param> <Repetitions> <MaxEvals>
    // Example: ./algo_exec cGA OneMax 100 23.5 30 10000

    if (argc < 7) {
        std::cerr << "Error: Insufficient arguments.\n";
        return 1;
    }

    std::string algo_name = argv[1];
    std::string prob_name = argv[2];
    int n = std::stoi(argv[3]);
    double param = std::stod(argv[4]);
    int repetitions = std::stoi(argv[5]);
    int max_evals = std::stoi(argv[6]);

    // 1. Resolve Problem
    FitnessFunction func = nullptr;
    if (prob_name == "OneMax")
        func = one_max;
    else if (prob_name == "LeadingOnes")
        func = leading_ones;
    else if (prob_name == "JumpK2")
        func = jump_k2;
    else if (prob_name == "JumpK3")
        func = jump_k3;
    else {
        std::cerr << "Unknown problem: " << prob_name << "\n";
        return 1;
    }

    // 2. Define Termination
    TerminationCriterion term_crit = [n, max_evals](int evals, double fitness) {
        return fitness >= n || evals >= max_evals;
    };

    // 3. Run Repetitions
    for (int r = 0; r < repetitions; ++r) {
        std::mt19937 gen(42 + r); // Reproducible seed
        BenchmarkResult res = {0, 0.0};
        std::string label = "Standard"; // Default label

        if (algo_name == "1+1_EA") {
            OnePlusOneEA solver(n);
            res = solver.run(func, term_crit, gen);
        } else if (algo_name == "cGA") {
            cGA solver(n, param);
            res = solver.run(func, term_crit, gen);
        } else if (algo_name == "sig-cGA-Original") {
            sigcGA solver(n, param, false);
            res = solver.run(func, term_crit, gen);
        } else if (algo_name == "sig-cGA-Simplified") {
            sigcGA solver(n, param, true);
            res = solver.run(func, term_crit, gen);
        } else {
            std::cerr << "Unknown algorithm: " << algo_name << "\n";
            return 1;
        }

        // Output CSV format: Algorithm,Problem,N,Param,Repetition,Evaluations,Success,BestFitness
        std::cout << algo_name << "," << prob_name << "," << n << "," << param << "," << r << "," << res.evaluations
                  << "," << (res.best_fitness >= n) << "," << res.best_fitness << "\n";
    }

    return 0;
}
