import optuna
import subprocess
import shlex
from concurrent.futures import ThreadPoolExecutor


def objective(trial):
    alp1RP = trial.suggest_float('alp1RP', 0.01, 0.99)
    alp2RP = trial.suggest_float('alp2RP', 0.01, 0.99)
    bet1RP = trial.suggest_float('bet1RP', 0.01, 50000.0)
    bet2RP = trial.suggest_float('bet2RP', 0.01, 50000.0)
    alp1SP = trial.suggest_float('alp1SP', 0.01, 0.99)
    alp2SP = trial.suggest_float('alp2SP', 0.01, 0.99)
    bet1SP = trial.suggest_float('bet1SP', 0.01, 50000.0)
    bet2SP = trial.suggest_float('bet2SP', 0.01, 50000.0)
    alp1PC = trial.suggest_float('alp1PC', 0.01, 0.99)
    alp2PC = trial.suggest_float('alp2PC', 0.01, 0.99)
    bet1PC = trial.suggest_float('bet1PC', 0.01, 50000.0)
    bet2PC = trial.suggest_float('bet2PC', 0.01, 50000.0)
    alp1SC = trial.suggest_float('alp1SC', 0.01, 0.99)
    alp2SC = trial.suggest_float('alp2SC', 0.01, 0.99)
    bet1SC = trial.suggest_float('bet1SC', 0.01, 50000.0)
    bet2SC = trial.suggest_float('bet2SC', 0.01, 50000.0)
    alp1PS = trial.suggest_float('alp1PS', 0.01, 0.99)
    alp2PS = trial.suggest_float('alp2PS', 0.01, 0.99)
    bet1PS = trial.suggest_float('bet1PS', 0.01, 50000.0)
    bet2PS = trial.suggest_float('bet2PS', 0.01, 50000.0)
    alp1PH = trial.suggest_float('alp1PH', 0.01, 0.99)
    alp2PH = trial.suggest_float('alp2PH', 0.01, 0.99)
    bet1PH = trial.suggest_float('bet1PH', 0.01, 50000.0)
    bet2PH = trial.suggest_float('bet2PH', 0.01, 50000.0)
    alp1IC = trial.suggest_float('alp1IC', 0.01, 0.99)
    alp2IC = trial.suggest_float('alp2IC', 0.01, 0.99)
    bet1IC = trial.suggest_float('bet1IC', 0.01, 50000.0)
    bet2IC = trial.suggest_float('bet2IC', 0.01, 50000.0)
    alp1SA = trial.suggest_float('alp1SA', 0.01, 0.99)
    alp2SA = trial.suggest_float('alp2SA', 0.01, 0.99)
    bet1SA = trial.suggest_float('bet1SA', 0.01, 50000.0)
    bet2SA = trial.suggest_float('bet2SA', 0.01, 50000.0)
    mpr = trial.suggest_float('mpr', 300.0, 500.0)
    mps = trial.suggest_float('mps', 15.0, 35.0)
    mpa = trial.suggest_float('mpa', 10.0, 15.0)
    mpd = trial.suggest_float('mpd', 15.0, 35.0)
    tpsc = trial.suggest_float('tpsc', 0.3, 0.7)
    mhc = trial.suggest_float('mhc', 80.0, 120.0)
    tic = trial.suggest_float('tic', 0.01, 0.99)
    msar = trial.suggest_float('msar', 100.0, 500.0)
    wRP = trial.suggest_float('wRP', 0.01, 0.99)
    wSP = trial.suggest_float('wSP', 0.01, 0.99)
    wPC = trial.suggest_float('wPC', 0.01, 0.99)
    wSC = trial.suggest_float('wSC', 0.01, 0.99)
    wPS = trial.suggest_float('wPS', 0.01, 0.99)
    wPH = trial.suggest_float('wPH', 0.01, 0.99)
    wIC = trial.suggest_float('wIC', 0.01, 0.99)
    wSA = trial.suggest_float('wSA', 0.01, 0.99)
    rtl = trial.suggest_float('rtl', 0.5, 0.99)

    
    
    # Correct indentation of the subprocess command
    command = (
        f"./legacy_detectors -a {alp1RP} -b {alp2RP} -c {bet1RP} -d {bet2RP} "
        f"-e {alp1SP} -f {alp2SP} -g {bet1SP} -h {bet2SP} -i {alp1PC} -j {alp2PC} "
        f"-k {bet1PC} -l {bet2PC} -m {alp1SC} -n {alp2SC} -o {bet1SC} -p {bet2SC} "
        f"-q {alp1PS} -r {alp2PS} -s {bet1PS} -t {bet2PS} -u {alp1PH} -v {alp2PH} "
        f"-w {bet1PH} -x {bet2PH} -y {alp1IC} -z {alp2IC} -A {bet1IC} -B {bet2IC} "
        f"-C {alp1SA} -D {alp2SA} -E {bet1SA} -F {bet2SA} -G {mpr} -H {mps} "
        f"-I {mpa} -J {mpd} -K {tpsc} -L {mhc} -M {tic} -N {msar} -O {wRP} "
        f"-P {wSP} -Q {wPC} -R {wSC} -S {wPS} -T {wPH} -U {wIC} -V {wSA} -W {rtl}"
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
