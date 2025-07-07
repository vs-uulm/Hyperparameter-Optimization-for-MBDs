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
pattern = 1
opzimizedValues = 0 # 0 = false, 1 = true
path = ""

def run():
    command = (
        f"../../MBD_Code/MBD_Code -A {architecture} -B {pattern} -C {opzimizedValues} -D {path} -r {train} -s {attackType}"
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





def write_list_to_file(strings, file_path):
    with open(file_path, 'w') as file:
        for string in strings:
            if string!=None:
                file.write(string + '\n')  # Write each string follow


if __name__ == "__main__":
    path = "../../VeReMi_Extension_Dataset"

    parser = argparse.ArgumentParser(description="Process command-line attributes.")

    # Add arguments
    parser.add_argument("-outputFile", type=str, required=True)
    parser.add_argument("-attackType", type=int, required=True)

    # Parse the arguments
    args = parser.parse_args()

    attackType = args.attackType
    run()

    write_list_to_file(results, args.outputFile)
