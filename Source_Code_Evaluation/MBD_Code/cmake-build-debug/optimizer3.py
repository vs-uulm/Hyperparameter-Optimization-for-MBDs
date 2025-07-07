import optuna
import subprocess
import shlex
from concurrent.futures import ThreadPoolExecutor


def objective(trial):
    var_range_plausibility = trial.suggest_float('varRangePlausibility', 0.01, 0.9)
    var_speed_plausibility = trial.suggest_float('varSpeedPlausibility', 0.01, 0.9)
    var_position_consistency = trial.suggest_float('varPositionConsistency', 0.01, 0.9)
    var_speed_consistency = trial.suggest_float('varSpeedConsistency', 0.01, 0.9)
    var_position_heading_consistency = trial.suggest_float('varPositionHeadingConsistency', 0.01, 0.9)
    var_intersection_check = trial.suggest_float('varIntersectionCheck', 0.01, 0.9)
    var_sudden_appearance_check = trial.suggest_float('varSuddenAppearanceCheck', 0.01, 0.9)
    var_rtl = trial.suggest_float('varRTL', 0.01, 0.9)

    command = (
        f"./legacy_detectors "
        f"-r {var_range_plausibility} "
        f"-s {var_speed_plausibility} "
        f"-p {var_position_consistency} "
        f"-c {var_speed_consistency} "
        f"-h {var_position_heading_consistency} "
        f"-i {var_intersection_check} "
        f"-a {var_sudden_appearance_check} "
        f"-t {var_rtl} "
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
    with ThreadPoolExecutor(max_workers=	n_threads) as executor:
        # Optimize the study using multiple threads
        study.optimize(objective, n_trials=n_trials, n_jobs=n_threads)

    # Print the best parameters
    print("Best parameters:", study.best_params)
    print("Best score:", study.best_value)

if __name__ == "__main__":
    run_optimization()
