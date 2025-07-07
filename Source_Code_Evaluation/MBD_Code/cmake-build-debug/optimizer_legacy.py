import optuna
import subprocess
import shlex
from concurrent.futures import ThreadPoolExecutor


def objective(trial):
    mpr = trial.suggest_float('mpr', 300.0, 500.0)
    mps = trial.suggest_float('mps', 15.0, 35.0)
    mpa = trial.suggest_float('mpa', 10.0, 15.0)
    mpd = trial.suggest_float('mpd', 15.0, 35.0)
    mhc = trial.suggest_float('mhc', 80.0, 120.0)
    msar = trial.suggest_float('msar', 100.0, 500.0)
    wRP = trial.suggest_float('wRP', 0.01, 1.0)
    wSP = trial.suggest_float('wSP', 0.01, 1.0)
    wPC = trial.suggest_float('wPC', 0.01, 1.0)
    wSC = trial.suggest_float('wSC', 0.01, 1.0)
    wPS = trial.suggest_float('wPS', 0.01, 1.0)
    wPH = trial.suggest_float('wPH', 0.01, 1.0)
    wIC = trial.suggest_float('wIC', 0.01, 1.0)
    wSA = trial.suggest_float('wSA', 0.01, 1.0)
    rtl = trial.suggest_float('rtl', 0.01, 8.0)

    
    
    # Correct indentation of the subprocess command
    command = (
        f"./legacy_detectors -a {mpr} -b {mps} "
        f"-c {mpa} -d {mpd} -e {mhc} -g {msar} -h {wRP} "
        f"-i {wSP} -j {wPC} -k {wSC} -l {wPS} -m {wPH} -n {wIC} -o {wSA} -p {rtl}"
    )

    # Ensure this subprocess call is correctly indented within the function
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Decode the process output
    output = process.stdout.decode('utf-8').strip().splitlines()
    precision = float(output[-1])
    recall = float(output[-2])


    return precision, recall

# Set up the study with multi-threading
def run_optimization():
    # Create a study object
    study = optuna.create_study(directions=["maximize", "maximize"])  # or 'minimize', depending on your objective
    
    # Define the number of trials and the number of threads
    n_trials = 10	
    n_threads = 10  # Adjust the number of threads based on your CPU cores

    # Use ThreadPoolExecutor for parallel execution
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        # Optimize the study using multiple threads
        study.optimize(objective, n_trials=n_trials, n_jobs=n_threads)

    # Print the best parameters
    trial_with_highest_accuracy = max(study.best_trials, key=lambda t: t.values[1])
    print(f"{trial_with_highest_accuracy.values}")

if __name__ == "__main__":
    run_optimization()
