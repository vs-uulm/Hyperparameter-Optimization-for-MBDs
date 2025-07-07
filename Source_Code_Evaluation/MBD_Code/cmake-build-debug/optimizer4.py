import optuna
import subprocess
import shlex
from concurrent.futures import ThreadPoolExecutor


def objective(trial):
    maxBelief1 = trial.suggest_float('maxBelief1', 0.01, 0.99)
    maxBelief2 = trial.suggest_float('maxBelief2', 0.01, 0.99)
    maxBelief3 = trial.suggest_float('maxBelief3', 0.01, 0.99)
    maxBelief4 = trial.suggest_float('maxBelief4', 0.01, 0.99)    
    maxBelief5 = trial.suggest_float('maxBelief5', 0.01, 0.99)
    maxBelief6 = trial.suggest_float('maxBelief6', 0.01, 0.99)
    maxBelief7 = trial.suggest_float('maxBelief7', 0.01, 0.99)
    maxBelief8 = trial.suggest_float('maxBelief8', 0.01, 0.99)
    maxDisbelief1 = trial.suggest_float('maxDisbelief1', 0.01, 0.99)
    maxDisbelief2 = trial.suggest_float('maxDisbelief2', 0.01, 0.99)
    maxDisbelief3 = trial.suggest_float('maxDisbelief3', 0.01, 0.99)
    maxDisbelief4 = trial.suggest_float('maxDisbelief4', 0.01, 0.99)    
    maxDisbelief5 = trial.suggest_float('maxDisbelief5', 0.01, 0.99)
    maxDisbelief6 = trial.suggest_float('maxDisbelief6', 0.01, 0.99)
    maxDisbelief7 = trial.suggest_float('maxDisbelief7', 0.01, 0.99)
    maxDisbelief8 = trial.suggest_float('maxDisbelief8', 0.01, 0.99)  
    
    
    # Correct indentation of the subprocess command
    command = (
        f"./legacy_detectors "
        f"-a {maxBelief1} "
        f"-b {maxBelief2} "
        f"-c {maxBelief3} "
        f"-d {maxBelief4} "
        f"-e {maxBelief5} "
        f"-f {maxBelief6} "
        f"-g {maxBelief7} "
        f"-h {maxBelief8} "
        f"-i {maxDisbelief1} "
        f"-j {maxDisbelief2} "
        f"-k {maxDisbelief3} "
        f"-l {maxDisbelief4} "
        f"-m {maxDisbelief5} "
        f"-n {maxDisbelief6} "
        f"-o {maxDisbelief7} "
        f"-p {maxDisbelief8} "
    )

    # Ensure this subprocess call is correctly indented within the function
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Decode the process output
    output = process.stdout.decode('utf-8').strip().splitlines()
    score = float(output[-1])

    return score

# Set up the study with multi-threading
def run_optimization():
    # Create a study object
    study = optuna.create_study(direction='maximize')  # or 'minimize', depending on your objective
    
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
