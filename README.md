# Estimation of Distribution Algorithms (EDAs)

This repository contains C++ implementations of:

* **(1+1) EA**: A baseline evolutionary algorithm.
* **Compact Genetic Algorithm (cGA)**: A standard estimation-of-distribution algorithm.
* **Significance-Based cGA (sig-cGA)**: An advanced EDA that reduces noise using history.

These algorithms are evaluated on **OneMax**, **LeadingOnes**, and **JumpK** benchmarks.

## Project Structure

* `src/`: C++ source code (Implementations).
* `include/`: Header files (Interfaces).
* `reproduce_results.py`: **Main entry point**. Python automation script that builds the C++ code, runs parallel experiments, and generates plots.
* `experiments_config.json`: Configuration file to control problem sizes, algorithms, and budgets.
* `plots/`: Generated PDF plots appear here.
* `results.csv`: Raw experimental data.

## Quick Start (Recommended)

The easiest way to run the project is using the Python automation script, which handles compilation (via CMake) and parallel execution.

### Prerequisites

* **Python 3.8+**
* **CMake 3.10+**
* **C++ Compiler** (GCC, Clang, or MSVC) supporting C++17.

### Installation & Run

1. **Set up Python Environment**

    ```bash
    # Linux/macOS
    python3 -m venv venv
    source venv/bin/activate

    # Windows (PowerShell)
    python -m venv venv
    .\venv\Scripts\Activate.ps1
    ```

2. **Install Dependencies**

    ```bash
    pip install -r requirements.txt
    ```

3. **Run Experiments**
    This command will build the C++ project, run benchmarks in parallel, and generate PDF plots.

    ```bash
    python reproduce_results.py
    ```

---

## Manual Build Instructions

If you prefer to compile and run the C++ executable manually:

### Linux / macOS

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
# Run specific benchmark: ./algo_exec <Algo> <Problem> <N> <Param> <Reps> <Budget>
./algo_exec cGA OneMax 100 23.5 1 10000
```

### Windows

```powershell
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
# Executable is usually in Release\algo_exec.exe
.\Release\algo_exec.exe cGA OneMax 100 23.5 1 10000
```

---

## Configuration (`experiments_config.json`)

You can modify experiment parameters without changing code by editing `experiments_config.json`.

* **`common_settings`**: Global settings like repetitions.
* **`algorithms`**: Enable/disable algorithms or change their parameters (e.g., `K` multipliers for cGA, `epsilon` for sig-cGA).
* **`problems`**: Define problem sizes (`n_values`), budgets, and difficulty formulas.

Example snippet:

```json
"cGA": {
    "enabled": true,
    "multipliers": [0.25, 0.5, 1.0] // Will test K = 0.25*Base, 0.5*Base...
}
```

## Benchmarks & Algorithms Details

1. **Algorithms**:

* **sig-cGA**: Implements **Equation (3)** for significance testing and **Algorithm 1** from the project description. Includes both *Original* (linked list) and *Simplified* (counter) history variants.
* **cGA**: Standard frequency update rule $p_i^{(t+1)} \leftarrow p_i^{(t)} + (x_i^{(t,1)}-x_i^{(t,2)}) / K$.
* **(1+1) EA**: Uses standard bit mutation with geometric distribution optimization for $O(n \log n)$ runtime.

2. **Problems**:

* **OneMax**: Linear slope, $O(n \log n)$ difficulty.
* **LeadingOnes**: Gradient only on prefix, $O(n^2)$ difficulty.
* **JumpK**: Valley crossing problem. $O(n^k)$ difficulty.
