import subprocess
import pprint
import shlex
from concurrent.futures import ThreadPoolExecutor
import json
import argparse

results = []
train = 0       # 0= false, 1 = true
attackType = 4 #1=const_pos, 2=rand_speed, 3=sudden_stop, 4=mixed

def read_and_process_file(line):
    record = json.loads(line.strip())
    mdi = record.get("parameters").get("MAX_DELTA_INTER")
    mhc = record.get("parameters").get("MAX_HEADING_CHANGE")
    mpa = record.get("parameters").get("MAX_PLAUSIBLE_ACCEL")
    mpd = record.get("parameters").get("MAX_PLAUSIBLE_DECEL")
    mpr = record.get("parameters").get("MAX_PLAUSIBLE_RANGE")
    mps = record.get("parameters").get("MAX_PLAUSIBLE_SPEED")
    msar = record.get("parameters").get("MAX_SA_RANGE")
    threshold = record.get("parameters").get("threshold")
    iteration = record.get("iteration")
    weightLegacyCheck = record.get("parameters", {}).get("weightLegacyCheck", [])
    weight_args = [float(weight) for weight in weightLegacyCheck]
    # Correct indentation of the subprocess command
    command = (
        f"./legacy_detectors -a {mpr} -b {mps} "
        f"-c {mpa} -d {mpd} -e {mhc} -f {mdi} -g {msar} -h {weight_args[0]} "
        f"-i {weight_args[1]} -j {weight_args[2]} -k {weight_args[3]} -l {weight_args[4]} -m {weight_args[5]} "
        f"-n {weight_args[6]} -o {weight_args[7]} -p {threshold} -q {iteration} -r {train} -s {attackType}"
    )

    # Ensure this subprocess call is correctly indented within the function
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Decode the process output
    output = process.stdout.decode('utf-8').strip().splitlines()
    mix = output[3]

    results.append(mix)



# Set up the study with multi-threading
def run_optimization(inputFile):
    n_threads = 10  # Adjust the number of threads based on your CPU cores

    # Use ThreadPoolExecutor for parallel execution
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        # Optimize the study using multiple threads
        with open(inputFile, 'r') as file:
            for line in file:
                read_and_process_file(line)



def write_list_to_file(strings, file_path):
    with open(file_path, 'w') as file:
        for string in strings:
            file.write(string + '\n')  # Write each string follow


if __name__ == "__main__":
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
