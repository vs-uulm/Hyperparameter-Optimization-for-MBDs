import csv
import json
import os
import argparse

def readGroundTruth(file_path):
    """
    Reads a JSON file and creates a dictionary with messageID as the key and
    position (pos) and speed (spd) as values.

    Args:
        file_path (str): Path to the JSON file.

    Returns:
        dict: Dictionary with messageID as keys and dictionaries with pos and spd as values.
    """
    # Initialize the map
    message_map = {}

    try:
        # Open and load the JSON file
        with open(file_path, 'r') as file:
            # Assuming the JSON file contains a list of objects
            for line_number, line in enumerate(file):
                data = json.loads(line)

                # Populate the map using messageID as the key
                message_id = data.get("messageID")
                pos = data.get("pos")
                spd = data.get("spd")
                message_map[message_id] = {"pos": pos, "spd": spd}


        print(f"Successfully created map with {len(message_map)} entries.")
        return message_map

    except FileNotFoundError:
        print(f"Error: File not found at {file_path}")
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON format in {file_path}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

    return {}

def read_files_in_directory(directory_path,type):
    """
    Iterates through all files in a directory, opens each file,
    and reads its contents line by line.

    Args:
        directory_path (str): Path to the directory containing files.
    """

    csvData = []
    try:
        # Iterate through all files in the directory
        for file_name in os.listdir(directory_path):
            file_path = os.path.join(directory_path, file_name)

            numMessages = 0
            numMaliciousMessages = 0
            sumRcvMsgDistance =0.0
            lastTimeStamp = 0.0
            avgRcvMsg = 0.0

            if "traceGroundTruthJSON" in file_name:
                continue

            # Check if it's a file (not a directory)
            if os.path.isfile(file_path):
                print(f"\nReading file: {file_name}")
                try:
                    with open(file_path, 'r') as file:
                        # Read file line by line
                        for line_number, line in enumerate(file):
                            if "messageID" in line:
                                numMessages = numMessages + 1
                                data = json.loads(line)

                                malicious = False
                                if data["messageID"] in groundTruthMap:
                                    if abs(data["pos"][0]-groundTruthMap[data["messageID"]]["pos"][0])>1 or abs(data["pos"][1]-groundTruthMap[data["messageID"]]["pos"][1])>1:
                                        malicious = True
                                    if abs(data["spd"][0]-groundTruthMap[data["messageID"]]["spd"][0])>1 and abs(data["spd"][1]-groundTruthMap[data["messageID"]]["spd"][1])>1:
                                        malicious = True

                                    if malicious == True:
                                        numMaliciousMessages = numMaliciousMessages + 1

                                    if lastTimeStamp<0.1:
                                        lastTimeStamp = data["rcvTime"]
                                    else:
                                        sumRcvMsgDistance = sumRcvMsgDistance + (data["rcvTime"] - lastTimeStamp)
                                        lastTimeStamp = data["rcvTime"]


                except Exception as e:
                    print(f"Error reading file {file_name}: {e}")
            else:
                print(f"Skipping non-file item: {file_name}")

            avgRcvMsg = sumRcvMsgDistance / numMessages
            csvData.append([file_name,avgRcvMsg,numMessages,numMaliciousMessages])
    except FileNotFoundError:
        print(f"Error: Directory not found at {directory_path}")
    except Exception as e:
        print(f"An error occurred: {e}")

    return csvData


# Example usage
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process command-line attributes.")

    # Add arguments
    parser.add_argument("-groundTruthFile", type=str, required=True)
    parser.add_argument("-path", type=str, required=True)
    parser.add_argument("-attack", type=str, required=True)
    parser.add_argument("-gt", type=str, required=True)

    args = parser.parse_args()

    groundTruthFile = args.groundTruthFile
    path = args.path
    gt = args.gt

    # Create the map
    groundTruthMap = readGroundTruth(groundTruthFile)
    csvData = read_files_in_directory(path,gt)

    with open(args.attack + "_output.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        # Write rows
        writer.writerows(csvData)




