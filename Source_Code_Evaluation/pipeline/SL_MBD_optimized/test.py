import subprocess
import pprint
import shlex
from concurrent.futures import ThreadPoolExecutor
import json
import argparse

results = []
train = 0       # 0 = false, 1 = true
attackType = 4  # 1 = const_pos, 2 = rand_speed, 3 = sudden_stop, 4 = mixed
architecture = 1
pattern = 3
opzimizedValues = 1 # 0 = false, 1 = true
path = ""

def read_and_process_file(line):
    record = json.loads(line.strip())
    mdi = record.get("parameters").get("MAX_DELTA_INTER")

    alpBRP = record.get("parameters").get("alpBRP")
    alpDRP = record.get("parameters").get("alpDRP")
    betBRP = record.get("parameters").get("betBRP")
    betDRP = record.get("parameters").get("betDRP")
    alpBSP = record.get("parameters").get("alpBSP")
    alpDSP = record.get("parameters").get("alpDSP")
    betBSP = record.get("parameters").get("betBSP")
    betDSP = record.get("parameters").get("betDSP")
    alpBPC = record.get("parameters").get("alpBPC")
    alpDPC = record.get("parameters").get("alpDPC")
    betBPC = record.get("parameters").get("betBPC")
    betDPC = record.get("parameters").get("betDPC")
    alpBSC = record.get("parameters").get("alpBSC")
    alpDSC = record.get("parameters").get("alpDSC")
    betBSC = record.get("parameters").get("betBSC")
    betDSC = record.get("parameters").get("betDSC")
    alpBPS = record.get("parameters").get("alpBPSC")
    alpDPS = record.get("parameters").get("alpDPSC")
    betBPS = record.get("parameters").get("betBPSC")
    betDPS = record.get("parameters").get("betDPSC")
    alpBPH = record.get("parameters").get("alpBPHC")
    alpDPH = record.get("parameters").get("alpDPHC")
    betBPH = record.get("parameters").get("betBPHC")
    betDPH = record.get("parameters").get("betDPHC")
    alpBIC = record.get("parameters").get("alpBIC")
    alpDIC = record.get("parameters").get("alpDIC")
    betBIC = record.get("parameters").get("betBIC")
    betDIC = record.get("parameters").get("betDIC")
    alpBSA = record.get("parameters").get("alpBSA")
    alpDSA = record.get("parameters").get("alpDSA")
    betBSA = record.get("parameters").get("betBSA")
    betDSA = record.get("parameters").get("betDSA")
    mpr = record.get("parameters").get("maxPlausilbeRange")
    mps = record.get("parameters").get("maxPlausibleSpeed")
    mpa = record.get("parameters").get("maxPlausibleAccel")
    mpd = record.get("parameters").get("maxPlausibleDecel")
    tpsc = record.get("parameters").get("thrPSC")
    mhc = record.get("parameters").get("maxHeadingChange")
    tic = record.get("parameters").get("thrIC")
    msar = record.get("parameters").get("maxSARange")
    thrB = record.get("parameters").get("thrBelief")
    thrD= record.get("parameters").get("thrDisbelief")
    thrU = record.get("parameters").get("thrUncertainty")
    mDist = record.get("parameters").get("maxDist")
    alpBPP = record.get("parameters").get("alpBPP")
    betBPP = record.get("parameters").get("betBPP")
    alpDPP = record.get("parameters").get("alpDPP")
    betDPP = record.get("parameters").get("betDPP")


    iteration = record.get("iteration")
    weightCatchCheck = record.get("parameters", {}).get("weightSLCheck", [])
    weight_args = [float(weight) for weight in weightCatchCheck]
    # Correct indentation of the subprocess command
    command = (
        f"../../MBD_Code/MBD_Code -A {architecture} -B {pattern} -C {opzimizedValues} -D {path} -E {alpBRP} -F {alpDRP} "
        f"-G {betBRP} -H {betDRP} -I {alpBSP} -J {alpDSP} -K {betBSP} -L {betDSP} -M {alpBPC} -N {alpDPC} -O {betBPC} -P {betDPC} -Q {alpBSC} "
        f"-R {alpDSC} -S {betBSC} -T {betDSC} -U {alpBPS} -V {alpDPS} -W {betBPS} -X {betDPS} -Y {alpBPH} -Z {alpDPH} -a {betBPH} -b {betDPH} "
        f"-c {alpBIC} -d {alpDIC} -e {betBIC} -f {betDIC} -g {alpBSA} -h {alpDSA} -i {betBSA} -j {betDSA} -k {mpr} -l {mps} -m {mpa} "
        f"-n {mpd} -o {tpsc} -p {mhc} -q {tic} -r {msar} -s {weight_args[0]} -t {weight_args[1]} -u {weight_args[2]} -v {weight_args[3]} -w {weight_args[4]} -x {weight_args[5]} "
        f"-y {weight_args[6]} -z {weight_args[7]} -aa {thrB} -ab {thrD} -ac {thrU} -ad {mDist} -ae {alpBPP} -af {betBPP} -ag {alpDPP} -ah {betDPP} -ai {weight_args[8]} -ba {train} -bb {attackType}"
    )

    # Ensure this subprocess call is correctly indented within the function
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Decode the process output
    output = process.stdout.decode('utf-8').strip().splitlines()

    if attackType>3:
        result = output[3]
    else:
        result = output[0]
    

    results.append(result)



# Set up the study with multi-threading
def run_optimization(inputFile):
    n_threads = 10  # Adjust the number of threads based on your CPU cores

    # Use ThreadPoolExecutor for parallel execution
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        # Optimize the study using multiple threads
        with open(inputFile, 'r') as file:
            future_results = [executor.submit(read_and_process_file, line) for line in file]
        
        # Collect results as they are completed
        for future in future_results:
            result = future.result()
            results.append(result)



def write_list_to_file(strings, file_path):
    with open(file_path, 'w') as file:
        for string in strings:
            if string!=None:
                file.write(string + '\n')  # Write each string follow


if __name__ == "__main__":
    path = "../../VeReMi_Extension_Dataset"

    parser = argparse.ArgumentParser(description="Process command-line attributes.")

    # Add arguments
    parser.add_argument("-inputFile", type=str, required=True)
    parser.add_argument("-outputFile", type=str, required=True)
    parser.add_argument("-attackType", type=int, required=True)

    # Parse the arguments
    args = parser.parse_args()

    attackType = args.attackType
    run_optimization(args.inputFile)

    write_list_to_file(results, args.outputFile)
