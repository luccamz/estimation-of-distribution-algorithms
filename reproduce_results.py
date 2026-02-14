import subprocess
import sys
import platform
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from pathlib import Path

# --- Configuration ---
BUILD_DIR = Path("build")
RESULTS_FILE = Path("results.csv")
PLOTS_DIR = Path("plots")
# Name of your executable defined in CMakeLists.txt (add_executable(algo_exec ...))
EXECUTABLE_NAME = "algo_exec"


def run_command(command, cwd=None, capture_output=False):
    """Runs a shell command and handles errors."""
    try:
        # On Windows, shell=True is often needed for cmake
        is_windows = platform.system() == "Windows"
        if capture_output:
            return subprocess.check_output(command, cwd=cwd, shell=is_windows)
        else:
            subprocess.check_call(command, cwd=cwd, shell=is_windows)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(command)}")
        print(f"Return code: {e.returncode}")
        sys.exit(1)


def build_project():
    """Configures and builds the C++ project using CMake."""
    print(f"--- Building Project in {BUILD_DIR} ---")
    BUILD_DIR.mkdir(exist_ok=True)

    # 1. Configure (Release mode is critical for benchmarking speed)
    cmd_config = ["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."]
    run_command(cmd_config, cwd=BUILD_DIR)

    # 2. Build
    cmd_build = ["cmake", "--build", ".", "--config", "Release"]
    run_command(cmd_build, cwd=BUILD_DIR)
    print("Build successful.\n")


def find_executable():
    """Locates the compiled binary across different OS/CMake layouts."""
    exe_name = (
        f"{EXECUTABLE_NAME}.exe" if platform.system() == "Windows" else EXECUTABLE_NAME
    )

    # Common paths where CMake might put the binary
    possible_paths = [
        BUILD_DIR / exe_name,
        BUILD_DIR / "src" / exe_name,
        BUILD_DIR / "Release" / exe_name,
        BUILD_DIR / "Debug" / exe_name,
    ]

    for path in possible_paths:
        if path.exists():
            return path.absolute()

    # Fallback: recursive search
    found = list(BUILD_DIR.rglob(exe_name))
    if found:
        return found[0].absolute()

    print(f"Error: Could not find executable '{exe_name}' in {BUILD_DIR}")
    sys.exit(1)


def run_experiments():
    """Runs the C++ executable and saves stdout to CSV."""
    exe_path = find_executable()
    print(f"--- Running Experiments using {exe_path.name} ---")
    print("This may take a few minutes depending on 'repetitions' and 'N'...")

    with open(RESULTS_FILE, "w") as outfile:
        # Run the executable and pipe stdout directly to the file
        subprocess.check_call([str(exe_path)], stdout=outfile)

    print(f"Experiments finished. Data saved to {RESULTS_FILE}\n")


def generate_plots():
    """Generates scientific plots from the CSV data."""
    print("--- Generating Plots ---")
    if not RESULTS_FILE.exists():
        sys.exit("Error: No results file found.")

    PLOTS_DIR.mkdir(exist_ok=True)

    try:
        df = pd.read_csv(RESULTS_FILE)
    except Exception as e:
        sys.exit(f"Failed to read CSV: {e}")

    # Set style
    sns.set_theme(style="whitegrid", context="paper", font_scale=1.2)

    # --- Plot 1: Runtime Scaling (Successes Only) ---
    success_df = df[df["Success"] == 1].copy()

    if success_df.empty:
        print("Warning: No successful runs found. Skipping runtime plots.")
    else:
        # We use a FacetGrid (relplot) to separate problems because they have
        # vastly different Y-scales (Jump is exponential, OneMax is linear-ish)
        g = sns.relplot(
            data=success_df,
            x="N",
            y="Evaluations",
            hue="Algorithm",
            col="Problem",  # Separate plot for each problem
            kind="line",
            marker="o",
            col_wrap=2,  # Wrap to next line if too many problems
            height=4,
            aspect=1.2,
            facet_kws={"sharey": False, "sharex": False},  # Independent axes
        )

        g.set_axis_labels("Problem Size (N)", "Evaluations")
        g.set_titles("{col_name}")

        # Set Log-Log scale for all plots (standard for algorithmic complexity)
        for ax in g.axes.flat:
            ax.set_xscale("log")
            ax.set_yscale("log")
            ax.grid(True, which="both", ls="-", alpha=0.2)

        save_path = PLOTS_DIR / "runtime_scaling.pdf"
        plt.savefig(save_path)
        print(f"Saved: {save_path}")

    # --- Plot 2: Success Rates ---
    # Calculate success rate per configuration
    success_rates = (
        df.groupby(["Algorithm", "Problem", "N"])["Success"].mean().reset_index()
    )

    g2 = sns.catplot(
        data=success_rates,
        x="N",
        y="Success",
        hue="Algorithm",
        col="Problem",
        kind="bar",
        col_wrap=3,
        height=4,
        aspect=1,
        sharex=False,
    )

    g2.set_axis_labels("Problem Size (N)", "Success Rate (0-1)")
    g2.set_titles("{col_name}")
    g2.set(ylim=(0, 1.1))  # Fix y-axis to 0-100%

    save_path_2 = PLOTS_DIR / "success_rates.pdf"
    plt.savefig(save_path_2)
    print(f"Saved: {save_path_2}")


if __name__ == "__main__":
    build_project()
    run_experiments()
    generate_plots()
    print("Done.")
