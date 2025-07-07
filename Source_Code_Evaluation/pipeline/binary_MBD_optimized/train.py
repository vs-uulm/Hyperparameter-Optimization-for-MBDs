import optuna
import subprocess
import pprint
import shlex
from concurrent.futures import ThreadPoolExecutor
from plotly.io import show



results = []
train = 1       # 0= false, 1 = true
attackType = 4 #1=const_pos, 2=rand_speed, 3=sudden_stop, 4=mixed
architecture = 1
pattern = 1
opzimizedValues = 1 # 0= false, 1 = true
path = "" # path to the root directory where the test and the train file are located

def objective(trial):

    mdi = trial.suggest_float('mdi', 0.1, 5.0)
    mpr = trial.suggest_float('mpr', 300.0, 500.0)
    mps = trial.suggest_float('mps', 15.0, 35.0)
    mpa = trial.suggest_float('mpa', 10.0, 15.0)
    mpd = trial.suggest_float('mpd', 15.0, 35.0)
    mhc = trial.suggest_float('mhc', 80.0, 120.0)
    msar = trial.suggest_float('msar', 100.0, 500.0)
    #wRP = trial.suggest_float('wRP', 0.01, 1.0)
    #wSP = trial.suggest_float('wSP', 0.01, 1.0)
    #wPC = trial.suggest_float('wPC', 0.01, 1.0)
    #wSC = trial.suggest_float('wSC', 0.01, 1.0)
    #wPS = trial.suggest_float('wPS', 0.01, 1.0)
    #wPH = trial.suggest_float('wPH', 0.01, 1.0)
    #wIC = trial.suggest_float('wIC', 0.01, 1.0)
    #wSA = trial.suggest_float('wSA', 0.01, 1.0)
    #wPP = trial.suggest_float('wPP', 0.01, 1.0)
    wRP = 1.0
    wSP = 1.0
    wPC = 1.0
    wSC = 1.0
    wPS = 1.0
    wPH = 1.0
    wIC = 1.0
    wSA = 1.0
    wPP = 1.0
    mdr = trial.suggest_float('mdr', 1.0, 20.0)
    rtl = trial.suggest_float('rtl', 0.01, 9.0)

    
    # Correct indentation of the subprocess command
    command = (
        f"../../MBD_Code/MBD_Code -A {architecture} -B {pattern} -C {opzimizedValues} -D {path} -a {mpr} -b {mps} "
        f"-c {mpa} -d {mpd} -e {mhc} -f {mdi} -g {msar} -h {wRP} "
        f"-i {wSP} -j {wPC} -k {wSC} -l {wPS} -m {wPH} -n {wIC} -o {wSA} -p {rtl} -q {trial.number} -r {train} -s {attackType} -t {wPP} -u {mdr}"
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
def run_optimization():
    # Create a study object
    study = optuna.create_study(directions=["maximize", "maximize"])  # or 'minimize', depending on your objective
    
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

    run_optimization()

    write_list_to_file(results, "output/train_precision_recall.json")
