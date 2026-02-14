#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
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

int main(int argc, char *argv[]) {
    std::string target_problem = "";
    if (argc > 1) {
        target_problem = argv[1];
    }

    // Header updated to include 'Label'
    std::cout << "Algorithm,Problem,N,Param,Repetition,Evaluations,Success,BestFitness,Label\n";

    const int REPETITIONS = 30;
    const std::vector<int> N_VALUES = {50, 100, 150, 200};

    std::vector<ProblemConfig> problems = {{"OneMax", one_max, 100.0, false},
                                           {"LeadingOnes", leading_ones, 15.0, true},
                                           {"JumpK2", jump_k2, 0, false},
                                           {"JumpK3", jump_k3, 0, false}};

    for (const auto &prob : problems) {
        if (!target_problem.empty() && prob.name != target_problem) {
            continue;
        }

        for (int n : N_VALUES) {
            // 1. Define Termination Logic
            int max_evals = 0;
            if (prob.name.find("Jump") != std::string::npos) {
                max_evals = 500000;
            } else {
                max_evals = prob.is_quadratic ? (prob.budget_mult * n * n) : (prob.budget_mult * n);
            }

            TerminationCriterion term_crit = [n, max_evals](int evals, double fitness) {
                return fitness >= n || evals >= max_evals;
            };

            // 2. (1+1) EA
            for (int r = 0; r < REPETITIONS; ++r) {
                std::mt19937 gen(42 + r);
                OnePlusOneEA solver(n);
                BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                std::cout << "1+1_EA," << prob.name << "," << n << ",0," << r << "," << res.evaluations << ","
                          << (res.best_fitness >= n) << "," << res.best_fitness << ",Standard\n";
            }

            // 3. cGA
            // Determine the base formula string and value
            std::string base_formula;
            double k_base;

            if (prob.name == "LeadingOnes") {
                k_base = n * std::pow(std::log(n), 2);
                base_formula = "n ln^2(n)";
            } else {
                k_base = std::sqrt(n) * std::log(n);
                base_formula = "sqrt(n) ln(n)";
            }

            struct KConfig {
                double mult;
                std::string label;
            };
            std::vector<KConfig> k_configs = {
                {0.5, "0.5 " + base_formula}, {1.0, base_formula}, {2.0, "2.0 " + base_formula}};

            for (const auto &cfg : k_configs) {
                double k_val = k_base * cfg.mult;
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    cGA solver(n, k_val);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "cGA," << prob.name << "," << n << "," << k_val << "," << r << "," << res.evaluations
                              << "," << (res.best_fitness >= n) << "," << res.best_fitness << "," << "K ~ " << cfg.label
                              << "\n";
                }
            }

            // 4. sig-cGA
            std::vector<double> eps_values = {1.0, 5.0, 10.0};
            for (double eps : eps_values) {
                std::stringstream ss;
                ss << "eps=" << std::fixed << std::setprecision(1) << eps;
                std::string eps_label = ss.str();

                // Simplified
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    sigcGA solver(n, eps, true);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "sig-cGA-Simp," << prob.name << "," << n << "," << eps << "," << r << ","
                              << res.evaluations << "," << (res.best_fitness >= n) << "," << res.best_fitness << ","
                              << eps_label << "\n";
                }

                // Original
                for (int r = 0; r < REPETITIONS; ++r) {
                    std::mt19937 gen(42 + r);
                    sigcGA solver(n, eps, false);
                    BenchmarkResult res = solver.run(prob.func, term_crit, gen);

                    std::cout << "sig-cGA-Orig," << prob.name << "," << n << "," << eps << "," << r << ","
                              << res.evaluations << "," << (res.best_fitness >= n) << "," << res.best_fitness << ","
                              << eps_label << "\n";
                }
            }
        }
    }
    return 0;
}
