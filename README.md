# Estimation of Distribution Algorithms (EDAs)

This repository contains C++ implementations of **$(1+1)$ EA**, **Compact Genetic Algorithm (cGA)**, and **Significance-Based cGA (sig-cGA)**. These algorithms are evaluated on **$\textsc{OneMax}$**, **$\textsc{LeadingOnes}$**, and **$\textsc{Jump}(k)$** benchmarks to analyze runtime performance and scalability. Conducted for the **CSC_42021_EP** course.

## Structure

```text
.
├── include/              # Interfaces (cga.hpp, sig_cga.hpp, etc.)
├── src/                  # Implementation (main.cpp, *.cpp)
├── plots/                # Output PDF plots
├── reproduce_results.py  # Automation script
├── requirements.txt      # Python dependencies
└── results.csv           # Generated data
```

## Requirements

* **C++**: Compiler supporting **C++17**, **CMake 3.10+**.
* **Python**: **3.8+** with `pandas`, `seaborn`, `matplotlib`.

## Usage

### Option 1: Automated Pipeline (Recommended)

Builds the project, runs benchmarks, and generates plots.

```bash
# 1. Setup Environment
python -m venv venv
source venv/bin/activate  # Windows: .\venv\Scripts\activate
pip install -r requirements.txt

# 2. Run Pipeline
python reproduce_results.py
```

### Option 2: Manual Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
./algo_exec > ../results.csv
```

## Algorithms & Benchmarks

* **Algorithms**:
  * **$(1+1)$ EA**: Baseline evolutionary algorithm.
  * **cGA**: Standard compact Genetic Algorithm.
  * **sig-cGA**: Reduces noise using history. Variants: *Simplified* (counts) and *Original* (subsequences).

* **Benchmarks**:
  * **$\textsc{OneMax}$**:
  * **$\textsc{LeadingOnes}$**:
  * **$\textsc{Jump}(k)$**: Multimodal valley of size $k$.

## Results

* **Data**: `results.csv` contains raw runtime metrics.
* **Plots**: `plots/` contains scaling analysis (`runtime_scaling.pdf`) and success probabilities (`success_rates.pdf`).
