import optuna
import subprocess
import pprint
import shlex
from concurrent.futures import ThreadPoolExecutor
from plotly.io import show
import numpy as np
import threading



num_iterations = 300
results = []
pareto_front = []
latest_results = []
train = 1       # 0= false, 1 = true
attackType = 4 #1=const_pos, 2=rand_speed, 3=sudden_stop, 4=mixed
architecture = 1
pattern = 3 # sl
opzimizedValues = 1 # 0= false, 1 = true
path = "" # path to the root directory where the test and the train file are located
counter = num_iterations
pareto_front = np.array([[0,0]])
sem = threading.Semaphore()
sem2 = threading.Semaphore()
maxIterations = 10000
iterations = 0

def objective(trial):

    global counter
    global iterations

    alpBRP = trial.suggest_float('alpBRP', 0.01, 0.95)
    alpDRP = trial.suggest_float('alpDRP', 0.01, 0.95)
    betBRP = trial.suggest_float('betBRP', 0.01, 5000000.0)
    betDRP = trial.suggest_float('betDRP', 0.01, 5000000.0)
    alpBSP = trial.suggest_float('alpBSP', 0.01, 0.95)
    alpDSP = trial.suggest_float('alpDSP', 0.01, 0.95)
    betBSP = trial.suggest_float('betBSP', 0.01, 5000000.0)
    betDSP = trial.suggest_float('betDSP', 0.01, 5000000.0)
    alpBPC = trial.suggest_float('alpBPC', 0.01, 0.95)
    alpDPC = trial.suggest_float('alpDPC', 0.01, 0.95)
    betBPC = trial.suggest_float('betBPC', 0.01, 5000000.0)
    betDPC = trial.suggest_float('betDPC', 0.01, 5000000.0)
    alpBSC = trial.suggest_float('alpBSC', 0.01, 0.95)
    alpDSC = trial.suggest_float('alpDSC', 0.01, 0.95)
    betBSC = trial.suggest_float('betBSC', 0.01, 5000000.0)
    betDSC = trial.suggest_float('betDSC', 0.01, 50000.0)
    alpBPS = trial.suggest_float('alpBPS', 0.01, 0.95)
    alpDPS = trial.suggest_float('alpDPS', 0.01, 0.95)
    betBPS = trial.suggest_float('betBPS', 0.01, 50000.0)
    betDPS = trial.suggest_float('betDPS', 0.01, 50000.0)
    alpBPH = trial.suggest_float('alpBPH', 0.01, 0.95)
    alpDPH = trial.suggest_float('alpDPH', 0.01, 0.95)
    betBPH = trial.suggest_float('betBPH', 0.01, 5000000.0)
    betDPH = trial.suggest_float('betDPH', 0.01, 5000000.0)
    alpBIC = trial.suggest_float('alpBIC', 0.01, 0.95)
    alpDIC = trial.suggest_float('alpDIC', 0.01, 0.95)
    betBIC = trial.suggest_float('betBIC', 0.01, 5000000.0)
    betDIC = trial.suggest_float('betDIC', 0.01, 5000000.0)
    alpBSA = trial.suggest_float('alpBSA', 0.01, 0.95)
    alpDSA = trial.suggest_float('alpDSA', 0.01, 0.95)
    betBSA = trial.suggest_float('betBSA', 0.01, 5000000.0)
    betDSA = trial.suggest_float('betDSA', 0.01, 5000000.0)
    mpr = trial.suggest_float('mpr', 300.0, 500.0)
    mps = trial.suggest_float('mps', 15.0, 35.0)
    mpa = trial.suggest_float('mpa', 10.0, 15.0)
    mpd = trial.suggest_float('mpd', 15.0, 35.0)
    tpsc = trial.suggest_float('tpsc', 0.3, 0.7)
    mhc = trial.suggest_float('mhc', 80.0, 120.0)
    tic = trial.suggest_float('tic', 0.01, 0.99)
    msar = trial.suggest_float('msar', 100.0, 500.0)
    wRP = 1.0
    wSP = 1.0
    wPC = 1.0
    wSC = 1.0
    wPS = 1.0
    wPH = 1.0
    wIC = 1.0
    wSA = 1.0
    wPP = 1.0
    thrB = trial.suggest_float('thrB', 0.5,0.99)
    thrD = 0.2
    thrU = 0.2
    mDist = trial.suggest_float('mDist',1.0, 20.0)
    alpBPP = trial.suggest_float('alpBPP', 0.01, 0.95)
    betBPP = trial.suggest_float('betBPP', 0.01, 5000000.0)
    alpDPP = trial.suggest_float('alpDPP', 0.01, 0.95)
    betDPP = trial.suggest_float('betDPP', 0.01, 5000000.0)

    
    # Correct indentation of the subprocess command
    command = (
        f"../../MBD_Code/MBD_Code -A {architecture} -B {pattern} -C {opzimizedValues} -D {path} -E {alpBRP} -F {alpDRP} "
        f"-G {betBRP} -H {betDRP} -I {alpBSP} -J {alpDSP} -K {betBSP} -L {betDSP} -M {alpBPC} -N {alpDPC} -O {betBPC} -P {betDPC} -Q {alpBSC} "
        f"-R {alpDSC} -S {betBSC} -T {betDSC} -U {alpBPS} -V {alpDPS} -W {betBPS} -X {betDPS} -Y {alpBPH} -Z {alpDPH} -a {betBPH} -b {betDPH} "
        f"-c {alpBIC} -d {alpDIC} -e {betBIC} -f {betDIC} -g {alpBSA} -h {alpDSA} -i {betBSA} -j {betDSA} -k {mpr} -l {mps} -m {mpa} "
        f"-n {mpd} -o {tpsc} -p {mhc} -q {tic} -r {msar} -s {wRP} -t {wSP} -u {wPC} -v {wSC} -w {wPS} -x {wPH} "
        f"-y {wIC} -z {wSA} -aa {thrB} -ab {thrD} -ac {thrU} -ad {mDist} -ae {alpBPP} -af {betBPP} -ag {alpDPP} -ah {betDPP} -ai {wPP} -ba {train} -bb {attackType} -ca {trial.number}"
    )

    # Ensure this subprocess call is correctly indented within the function
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Decode the process output
    output = process.stdout.decode('utf-8').strip().splitlines()
    const_pos = output[0]
    rand_speed = output[1]
    sudden_stop = output [2]
    mix = output[3]
    precision = float(output[4])
    recall = float(output[5])

    results.append(const_pos)
    results.append(rand_speed)
    results.append(sudden_stop)
    results.append(mix)


    return precision, recall


# Set up the study with multi-threading
def run_optimization(study):
    # Create a study object
    
    # Define the number of trials and the number of threads
    n_trials = 100000
    n_threads = 10  # Adjust the number of threads based on your CPU cores

    # Use ThreadPoolExecutor for parallel execution
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        # Optimize the study using multiple threads
        study.optimize(objective, n_trials=n_trials, n_jobs=n_threads)


def write_list_to_file(strings, file_path):
    with open(file_path, 'w') as file:
        for string in strings:
            file.write(string + '\n')  # Write each string follow    


if __name__ == "__main__":
    path = "../../VeReMi_Extension_Dataset"
    study = optuna.create_study(directions=["maximize", "maximize"])  # or 'minimize', depending on your objective

    run_optimization(study)


    write_list_to_file(results, "output/train_precision_recall.json")
