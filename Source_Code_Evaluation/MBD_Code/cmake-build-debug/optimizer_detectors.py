import optuna
import subprocess
import shlex
from concurrent.futures import ThreadPoolExecutor


def objective(trial):
    var_range_plausibility = trial.suggest_float('varRangePlausibility', 0.000001, 0.99)
    #var_max_plausible_range = trial.suggest_float('MAX_PLAUSIBLE_RANGE', 100, 500)
    #var_max_plausible_decel = trial.suggest_float('MAX_PLAUSIBLE_DECEL', 0.75, 0.99)


    command = (
        f"./legacy_detectors "
        f"-v {var_range_plausibility} "
        #f"-m {var_max_plausible_range}  "
        #f"-d {var_max_plausible_decel}  "
    )
    
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    

    output = process.stdout.decode('utf-8').strip().splitlines()
    score = float(output[-1])

    return score

# Set up the study with multi-threading
def run_optimization():
    # Create a study object
    study = optuna.create_study(direction='maximize')  # or 'maximize', depending on your objective
    
    # Define the number of trials and the number of threads
    n_trials = 300
    n_threads = 10  # Adjust the number of threads based on your CPU cores

    # Use ThreadPoolExecutor for parallel execution
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        # Optimize the study using multiple threads
        study.optimize(objective, n_trials=n_trials, n_jobs=n_threads)

    # Print the best parameters
    print("Best parameters:", study.best_params)
    print("Best score:", study.best_value)

if __name__ == "__main__":
    run_optimization()
