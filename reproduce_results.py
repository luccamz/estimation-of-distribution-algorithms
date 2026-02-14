import subprocess
import sys
import platform
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
from pathlib import Path
import multiprocessing
import json
import math
import time

# --- Configuration ---
BUILD_DIR = Path("build")
RESULTS_FILE = Path("results.csv")
PLOTS_DIR = Path("plots")
CONFIG_FILE = Path("experiments_config.json")
EXECUTABLE_NAME = "algo_exec"


def run_command(command, cwd=None):
    try:
        is_windows = platform.system() == "Windows"
        subprocess.check_call(command, cwd=cwd, shell=is_windows)
    except subprocess.CalledProcessError as e:
        print(f"Error: {' '.join(command)}")
        sys.exit(1)


def build_project():
    print(f"--- Building Project in {BUILD_DIR} ---")
    BUILD_DIR.mkdir(exist_ok=True)
    run_command(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."], cwd=BUILD_DIR)
    run_command(["cmake", "--build", ".", "--config", "Release"], cwd=BUILD_DIR)
    print("Build successful.\n")


def find_executable():
    exe_name = (
        f"{EXECUTABLE_NAME}.exe" if platform.system() == "Windows" else EXECUTABLE_NAME
    )
    found = list(BUILD_DIR.rglob(exe_name))
    if found:
        return found[0].absolute()
    sys.exit(f"Error: Could not find executable '{exe_name}'")


def calculate_budget(n, settings):
    b_type = settings["budget_type"]
    if b_type == "linear":
        return int(n * settings["budget_multiplier"])
    elif b_type == "quadratic":
        return int(n * n * settings["budget_multiplier"])
    elif b_type == "fixed":
        return int(settings["budget_fixed"])
    return 100000


def calculate_k(n, formula_str):
    allowed_names = {"n": n, "sqrt": math.sqrt, "log": math.log, "pow": math.pow}
    return eval(formula_str, {"__builtins__": {}}, allowed_names)


def generate_tasks():
    if not CONFIG_FILE.exists():
        sys.exit(f"Config file {CONFIG_FILE} not found.")

    with open(CONFIG_FILE) as f:
        config = json.load(f)

    tasks = []
    reps = config["common_settings"]["repetitions"]
    n_values = config["common_settings"]["n_values"]

    for prob_name, prob_settings in config["problems"].items():
        for n in n_values:
            max_evals = calculate_budget(n, prob_settings)

            # 1. (1+1) EA
            if config["algorithms"]["1+1_EA"]["enabled"]:
                tasks.append(
                    {
                        "id": f"1+1_EA_{prob_name}_{n}",
                        "args": [
                            "1+1_EA",
                            prob_name,
                            str(n),
                            "0",
                            str(reps),
                            str(max_evals),
                        ],
                        "meta": {"label": "$(1+1)$ EA"},
                    }
                )

            # 2. cGA
            if config["algorithms"]["cGA"]["enabled"]:
                k_base = calculate_k(n, prob_settings["k_formula"])
                multipliers = config["algorithms"]["cGA"]["multipliers"]
                for mult in multipliers:
                    k_val = k_base * mult
                    tasks.append(
                        {
                            "id": f"cGA_{prob_name}_{n}_K{mult}",
                            "args": [
                                "cGA",
                                prob_name,
                                str(n),
                                str(k_val),
                                str(reps),
                                str(max_evals),
                            ],
                            "meta": {
                                "label": f"cGA ($K \\approx {mult}\\cdot K_{{base}}$)"
                            },
                        }
                    )

            # 3. sig-cGA
            if config["algorithms"]["sig-cGA"]["enabled"]:
                eps_values = config["algorithms"]["sig-cGA"]["values"]
                variants = config["algorithms"]["sig-cGA"]["variants"]
                for var in variants:
                    algo_id = f"sig-cGA-{var}"
                    for eps in eps_values:
                        tasks.append(
                            {
                                "id": f"{algo_id}_{prob_name}_{n}_eps{eps}",
                                "args": [
                                    algo_id,
                                    prob_name,
                                    str(n),
                                    str(eps),
                                    str(reps),
                                    str(max_evals),
                                ],
                                "meta": {"label": f"sig-cGA {var} ($\\epsilon={eps}$)"},
                            }
                        )
    return tasks


def run_worker(task):
    """Worker function: Runs one config and returns the CSV lines."""
    exe_path = find_executable()
    cmd = [str(exe_path)] + task["args"]

    # We do NOT print here to avoid race conditions/garbled text in parallel output.
    # Instead, we return success status to the main loop.
    try:
        output = subprocess.check_output(cmd, text=True).strip()
        lines = output.splitlines()
        labeled_lines = [f"{line},{task['meta']['label']}" for line in lines]
        return (task["id"], labeled_lines)
    except subprocess.CalledProcessError as e:
        return (task["id"], [])


def run_experiments():
    print(f"--- Running Experiments from {CONFIG_FILE} ---")
    tasks = generate_tasks()
    total_tasks = len(tasks)

    # Dynamic Parallelization: Use all available cores
    num_workers = min(multiprocessing.cpu_count(), total_tasks)
    print(f"Generated {total_tasks} tasks.")
    print(f"Running on {num_workers} parallel threads...\n")

    results = []
    header = (
        "Algorithm,Problem,N,Param,Repetition,Evaluations,Success,BestFitness,Label"
    )

    # Use imap_unordered to get results as they finish (Real-time progress)
    with multiprocessing.Pool(num_workers) as pool:
        for i, (task_id, lines) in enumerate(pool.imap_unordered(run_worker, tasks), 1):
            if lines:
                results.extend(lines)
                print(f"[{i}/{total_tasks}] Finished: {task_id}")
            else:
                print(f"[{i}/{total_tasks}] FAILED:   {task_id}")

    with open(RESULTS_FILE, "w") as f:
        f.write(header + "\n")
        f.write("\n".join(results))
    print(f"\nSaved results to {RESULTS_FILE}\n")


def generate_plots():
    print("--- Generating Plots ---")
    if not RESULTS_FILE.exists():
        sys.exit("No results")

    df = pd.read_csv(RESULTS_FILE)

    # Setup internal math rendering (Computer Modern font)
    sns.set_theme(
        style="whitegrid",
        context="paper",
        font_scale=1.2,
        rc={"mathtext.fontset": "cm", "font.family": "serif"},
    )

    PLOTS_DIR.mkdir(exist_ok=True)

    for problem in df["Problem"].unique():
        print(f"Plotting {problem}...")
        prob_df = df[df["Problem"] == problem]
        unique_n = sorted(prob_df["N"].unique())

        # Scaling Plot
        success_df = prob_df[prob_df["Success"] == 1]
        if not success_df.empty:
            plt.figure(figsize=(10, 6))
            ax = sns.lineplot(
                data=success_df,
                x="N",
                y="Evaluations",
                hue="Label",
                style="Label",
                markers=True,
                dashes=False,
                errorbar="sd",
                linewidth=2,
                markersize=8,
            )
            ax.set_title(rf"Runtime Scaling: {problem}", pad=15, fontweight="bold")
            ax.set_xscale("log")
            ax.set_yscale("log")
            ax.set_xticks(unique_n)
            ax.get_xaxis().set_major_formatter(ScalarFormatter())
            ax.minorticks_off()
            plt.legend(
                bbox_to_anchor=(1.02, 1), loc="upper left", title="Configuration"
            )
            plt.grid(True, which="major", ls="-", alpha=0.5)
            plt.tight_layout()
            plt.savefig(PLOTS_DIR / f"{problem}_scaling.pdf")
            plt.close()

        # Success Plot
        plt.figure(figsize=(10, 6))
        # Ensure we count all rows including failures
        success_rates = prob_df.groupby(["Label", "N"])["Success"].mean().reset_index()
        ax = sns.barplot(data=success_rates, x="N", y="Success", hue="Label")
        ax.set_title(rf"Success Rates: {problem}", pad=15, fontweight="bold")
        ax.set_ylim(0, 1.05)
        plt.legend(bbox_to_anchor=(1.02, 1), loc="upper left", title="Configuration")
        plt.tight_layout()
        plt.savefig(PLOTS_DIR / f"{problem}_success.pdf")
        plt.close()


if __name__ == "__main__":
    build_project()
    run_experiments()
    generate_plots()
    print("Done.")
