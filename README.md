# Estimation-of-Distribution Algorithms (EDAs) Optimization Project

<!--toc:start-->
- [Estimation-of-Distribution Algorithms (EDAs) Optimization Project](#estimation-of-distribution-algorithms-edas-optimization-project)
  - [Overview](#overview)
  - [Implemented Algorithms](#implemented-algorithms)
  - [Benchmarks](#benchmarks)
  - [Requirements](#requirements)
  - [Build Instructions](#build-instructions)
  - [Experimental Setup](#experimental-setup)
  - [Project Structure](#project-structure)
<!--toc:end-->

## Overview

This project implements and studies the **significance-based compact genetic algorithm (sig-cGA)**.
We evaluate its performance against the **compact genetic algorithm (cGA)** and the **(1+1) evolutionary algorithm (EA)** across various pseudo-Boolean benchmark functions.

## Implemented Algorithms

1. **sig-cGA**: An EDA using a significance-based probabilistic model update.
2. **cGA**: A classic EDA utilizing a hypothetical population size $K$.
3. **(1+1) EA**: A baseline evolutionary algorithm using standard bit mutation.

## Benchmarks

The algorithms are evaluated on the following functions with an all-1s optimum:

- **OneMax**: Measures the number of 1s in a bit string.
- **LeadingOnes**: Counts the number of consecutive 1s starting from the first bit.
- **Jump**: A OneMax-style function with a challenging plateau of local optima.

## Requirements

- C++17 or higher
- CMake (minimum version 3.10)

## Build Instructions

1. Create a build directory: `mkdir build && cd build`
2. Configure the project: `cmake ..`
3. Build the executable: `cmake --build .`
4. Run experiments: `./algo_exec`

## Experimental Setup

- **Reproducibility**: All experiments use fixed seeds for random number generators.
- **Problem Sizes**: Minimum $n = 100$.
- **Metrics**: We measure the number of fitness function evaluations until the global optimum is found.

## Project Structure

- `src/`: Core algorithm implementations and benchmark definitions.
- `include/`: Header files for data structures [Individuals, Frequency Vectors].
- `results/`: CSV files containing experimental data.
- `report.pdf`: Detailed scientific report and analysis of findings.
