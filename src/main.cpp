#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "benchmarks.hpp"
#include "cga.hpp"
#include "one_plus_one_ea.hpp"
#include "optimizer.hpp"
#include "sig_cga.hpp"

// --- Benchmark Wrappers ---
double jump_k2(const Individual &ind) { return jump(ind, 2); }
double jump_k3(const Individual &ind) { return jump(ind, 3); }

// --- Configuration ---
struct ProblemConfig {
    std::string name;
    FitnessFunction func;
    double budget_mult;
    bool is_quadratic;
};

int main() {
    std::cout << "Algorithm,Problem,N,Param,Repetition,Evaluations,Success\n";

    const int REPETITIONS = 30;
    const std::vector<int> N_VALUES = {100, 200};

    std::vector<ProblemConfig> problems = {{"OneMax", one_max, 100.0, false},
                                           {"LeadingOnes", leading_ones, 15.0, true},
                                           {"JumpK2", jump_k2, 0, false},
                                           {"JumpK3", jump_k3, 0, false}};

    for (const auto &prob : problems) {
        for (int n : N_VALUES) {
            // 1. Define Termination Logic Locally (Capturing n and budget)
            int max_evals = 0;
            if (prob.name.find("Jump") != std::string::npos) {
                max_evals = 500000;
            } else {
                max_evals = prob.is_quadratic ? (prob.budget_mult * n * n) : (prob.budget_mult * n);
            }

            // This lambda replaces the global function pointer
            TerminationCriterion term_crit = [n, max_evals](int evals, double fitness) {
                return fitness >= n || evals >= max_evals;
            };

            // 2. (1+1) EA
            for (int r = 0; r < REPETITIONS; ++r) {
                std::mt19937 gen(42 + r);
                OnePlusOneEA solver(n);
                BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                std::cout << "1+1_EA," << prob.name << "," << n << ",0," << r << "," << res.evaluations << ","
                          << (res.best_fitness >= n) << "\n";
            }

            // 3. cGA
            double k_base = (prob.name == "LeadingOnes") ? n * std::pow(std::log(n), 2) : std::sqrt(n) * std::log(n);

            std::vector<double> k_values = {k_base * 0.5, k_base, k_base * 2.0};

            for (double k : k_values) {
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    cGA solver(n, k);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "cGA," << prob.name << "," << n << "," << k << "," << r << "," << res.evaluations
                              << "," << (res.best_fitness >= n) << "\n";
                }
            }

            // 4. sig-cGA
            std::vector<double> eps_values = {1.0, 5.0, 10.0};

            for (double eps : eps_values) {
                // Simplified
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    sigcGA solver(n, eps, true);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "sig-cGA-Simp," << prob.name << "," << n << "," << eps << "," << r << ","
                              << res.evaluations << "," << (res.best_fitness >= n) << "\n";
                }

                // Original
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    sigcGA solver(n, eps, false);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "sig-cGA-Orig," << prob.name << "," << n << "," << eps << "," << r << ","
                              << res.evaluations << "," << (res.best_fitness >= n) << "\n";
                }
            }
        }
    }
    return 0;
}
