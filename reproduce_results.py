import subprocess
import sys
import platform
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
from pathlib import Path
import multiprocessing

# --- Configuration ---
BUILD_DIR = Path("build")
RESULTS_FILE = Path("results.csv")
PLOTS_DIR = Path("plots")
EXECUTABLE_NAME = "algo_exec"
PROBLEMS = ["OneMax", "LeadingOnes", "JumpK2", "JumpK3"]


def run_command(command, cwd=None):
    """Runs a shell command and checks for errors."""
    try:
        is_windows = platform.system() == "Windows"
        subprocess.check_call(command, cwd=cwd, shell=is_windows)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)


def build_project():
    """Configures and builds the C++ project."""
    print(f"--- Building Project in {BUILD_DIR} ---")
    BUILD_DIR.mkdir(exist_ok=True)

    cmd_config = ["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."]
    run_command(cmd_config, cwd=BUILD_DIR)

    cmd_build = ["cmake", "--build", ".", "--config", "Release"]
    run_command(cmd_build, cwd=BUILD_DIR)
    print("Build successful.\n")


def find_executable():
    """Locates the compiled binary."""
    exe_name = (
        f"{EXECUTABLE_NAME}.exe" if platform.system() == "Windows" else EXECUTABLE_NAME
    )

    possible_paths = [
        BUILD_DIR / exe_name,
        BUILD_DIR / "src" / exe_name,
        BUILD_DIR / "Release" / exe_name,
    ]
    for path in possible_paths:
        if path.exists():
            return path.absolute()

    found = list(BUILD_DIR.rglob(exe_name))
    if found:
        return found[0].absolute()

    sys.exit(f"Error: Could not find executable '{exe_name}'")


def run_single_benchmark(problem):
    """Worker function to run a single benchmark."""
    exe_path = find_executable()
    print(f"Starting benchmark: {problem}...")
    try:
        result = subprocess.check_output([str(exe_path), problem], text=True)
        print(f"Finished: {problem}")
        return result.strip().splitlines()
    except subprocess.CalledProcessError as e:
        print(f"FAILED: {problem} - {e}")
        return []


def run_experiments():
    """Runs benchmarks in parallel."""
    print(f"--- Running Experiments (Parallel) ---")

    num_workers = min(len(PROBLEMS), multiprocessing.cpu_count())
    print(f"Using {num_workers} parallel workers.")

    all_output_lines = []

    with multiprocessing.Pool(num_workers) as pool:
        results = pool.map(run_single_benchmark, PROBLEMS)

        for lines in results:
            if not lines:
                continue
            if not all_output_lines:
                all_output_lines.extend(lines)
            elif len(lines) > 1:
                all_output_lines.extend(lines[1:])

    with open(RESULTS_FILE, "w") as f:
        f.write("\n".join(all_output_lines))
    print(f"\nData saved to {RESULTS_FILE}\n")


def format_math_label(row):
    """Creates a Mathtext-formatted label for the legend."""
    algo = row["Algorithm"]
    raw_label = row[
        "Label"
    ]  # Comes from C++ (e.g., "K ~ 0.5 sqrt(n) ln(n)", "eps=1.0")

    # 1. Clean Algorithm Name
    # Replace "sig-cGA-" with "sig-cGA " and underscores with spaces
    clean_algo = algo.replace("sig-cGA-", "sig-cGA ").replace("_", " ")

    if "1+1" in algo:
        # Use simple mathtext for (1+1)
        return r"$(1+1)$ EA"

    # 2. Convert raw label parameters to Python Mathtext
    # We wrap the math parts in $...$

    tex_label = raw_label
    tex_label = tex_label.replace("eps=", r"\epsilon=")
    tex_label = tex_label.replace("K ~", r"K \approx")  # Use approx symbol
    tex_label = tex_label.replace("sqrt(n)", r"\sqrt{n}")
    tex_label = tex_label.replace("ln(n)", r"\ln(n)")
    tex_label = tex_label.replace("ln^2(n)", r"\ln^2(n)")

    # Return formatted string: "sig-cGA ($ \epsilon=1.0 $)"
    return rf"{clean_algo} (${tex_label}$)"


def generate_plots():
    """Generates PDF plots using internal Mathtext rendering."""
    print("--- Generating Plots ---")
    if not RESULTS_FILE.exists():
        sys.exit("Error: No results file found.")

    PLOTS_DIR.mkdir(exist_ok=True)

    try:
        df = pd.read_csv(RESULTS_FILE)
    except pd.errors.EmptyDataError:
        sys.exit("Error: Results file is empty.")

    # Apply formatting
    df["LegendLabel"] = df.apply(format_math_label, axis=1)

    # Configure Seaborn/Matplotlib for internal Math rendering
    # 'mathtext.fontset': 'cm' makes the math look like LaTeX (Computer Modern)
    sns.set_theme(
        style="whitegrid",
        context="paper",
        font_scale=1.2,
        rc={"mathtext.fontset": "cm", "font.family": "serif"},
    )

    unique_problems = df["Problem"].unique()

    for problem in unique_problems:
        print(f"Plotting {problem}...")
        prob_df = df[df["Problem"] == problem]
        unique_n = sorted(prob_df["N"].unique())

        # --- Plot 1: Runtime Scaling ---
        success_df = prob_df[prob_df["Success"] == 1]

        if not success_df.empty:
            plt.figure(figsize=(9, 6))
            ax = sns.lineplot(
                data=success_df,
                x="N",
                y="Evaluations",
                hue="LegendLabel",
                style="LegendLabel",
                markers=True,
                dashes=False,
                errorbar="sd",
                linewidth=2.0,
                markersize=8,
                alpha=0.9,
            )

            # Using r-strings for titles/labels to support simple math if needed
            ax.set_title(rf"Runtime Scaling: {problem}", pad=15, fontweight="bold")
            ax.set_ylabel("Evaluations (Log Scale)")
            ax.set_xlabel(r"Problem Size ($N$)")
            ax.set_xscale("log")
            ax.set_yscale("log")

            # Fix X-axis ticks
            ax.set_xticks(unique_n)
            ax.get_xaxis().set_major_formatter(ScalarFormatter())
            ax.minorticks_off()

            # Place legend outside
            plt.legend(
                bbox_to_anchor=(1.02, 1),
                loc="upper left",
                borderaxespad=0,
                title="Algorithm Configuration",
            )
            plt.grid(True, which="major", ls="-", alpha=0.5)
            plt.tight_layout()
            plt.savefig(PLOTS_DIR / f"{problem}_scaling.pdf")
            plt.close()

        # --- Plot 2: Success Rates ---
        plt.figure(figsize=(9, 6))
        success_rates = (
            prob_df.groupby(["LegendLabel", "N"])["Success"].mean().reset_index()
        )

        ax = sns.barplot(
            data=success_rates, x="N", y="Success", hue="LegendLabel", alpha=0.9
        )

        ax.set_title(rf"Success Rates: {problem}", pad=15, fontweight="bold")
        ax.set_ylabel("Success Rate")
        ax.set_ylim(0, 1.05)
        ax.set_xlabel(r"Problem Size ($N$)")
        plt.legend(
            bbox_to_anchor=(1.02, 1),
            loc="upper left",
            borderaxespad=0,
            title="Algorithm Configuration",
        )
        plt.tight_layout()
        plt.savefig(PLOTS_DIR / f"{problem}_success.pdf")
        plt.close()

    print(f"Plots saved to {PLOTS_DIR}/")


if __name__ == "__main__":
    build_project()
    run_experiments()
    generate_plots()
    print("Done.")
