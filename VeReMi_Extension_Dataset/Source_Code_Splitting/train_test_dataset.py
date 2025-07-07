from scipy.stats import ks_2samp
import numpy as np
import csv
import random
import argparse

def perform_two_sample_ks_test(sample1, sample2):
    """
    Perform a two-sample Kolmogorov-Smirnov test to compare two distributions.

    Args:
        sample1 (array-like): First sample of data.
        sample2 (array-like): Second sample of data.

    Returns:
        dict: Contains KS statistic and p-value.
    """
    # Perform the KS test
    ks_statistic, p_value = ks_2samp(sample1, sample2)

    print(p_value)
    if p_value < 0.05:
        return False
    else:
        return True

def read_csv_to_2d_array(file_path):
    two_d_array = []

    with open(file_path, 'r') as file:
        csv_reader = csv.reader(file)

        # Iterate through each line in the CSV and append to the array
        for row in csv_reader:
            two_d_array.append([row[0],float(row[1]),int(row[2]),int(row[3])])

    return two_d_array


def split_array_by_sum(data, target_ratio, random_seed=None):
    if random_seed is not None:
        random.seed(random_seed)

    # Shuffle data for randomness
    data = data.copy()
    random.shuffle(data)

    # Calculate total sum of the second elements
    total_sum = sum(row[2] for row in data)

    # Initialize subsets
    set1, set2 = [], []
    set1_sum, set2_sum = 0, 0

    # Distribute rows between the two sets
    for row in data:
        if set1_sum / total_sum <= target_ratio:
            set1.append(row)
            set1_sum += row[2]
        else:
            set2.append(row)
            set2_sum += row[2]

    return set1, set2

def split_into_three_arrays(array1, array2):
    # Split array1 into three separate arrays
    array1_col1 = [row[1] for row in array1]
    array1_col2 = [row[2] for row in array1]
    array1_col3 = [row[3] for row in array1]

    # Split array2 into three separate arrays
    array2_col1 = [row[1] for row in array2]
    array2_col2 = [row[2] for row in array2]
    array2_col3 = [row[3] for row in array2]

    return array1_col1, array1_col2, array1_col3, array2_col1, array2_col2, array2_col3


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process command-line attributes.")
    parser.add_argument("-file", type=str, required=True)
    parser.add_argument("-attack", type=str, required=True)
    parser.add_argument("-division", type=float, required=True)

    args = parser.parse_args()
    file_path = args.file
    attack = args.attack
    division = args.division

    data = read_csv_to_2d_array(file_path)

    rcvRateKS_p_max=0
    rcvRateKS_d_max=0
    sumMsgsKS_p_max=0
    sumMsgsKS_d_max=0
    sumMalMsgsKS_p_max=0
    sumMalMsgsKS_d_max=0
    train_maxVal = []
    test_maxVal = []


    for x in range(30000):
        set1, set2 = split_array_by_sum(data,division)
        rcvRate1, sumMsgs1, sumMalMsgs1, rcvRate2, sumMsgs2, sumMalMsgs2 = split_into_three_arrays(set1,set2)

        # Perform the KS test
        rcvRateKS_d, rcvRateKS_p = ks_2samp(rcvRate1, rcvRate2)
        sumMsgsKS_d, sumMsgsKS_p = ks_2samp(sumMsgs1, sumMsgs2)
        sumMalMsgsKS_d, sumMalMsgsKS_p = ks_2samp(sumMalMsgs1, sumMalMsgs2)

        # Display the results
        if (rcvRateKS_p>0.05 and sumMsgsKS_p > 0.05 and sumMalMsgsKS_p > 0.05):
            if (rcvRateKS_p>rcvRateKS_p_max and sumMsgsKS_p > sumMsgsKS_p_max and sumMalMsgsKS_p > sumMalMsgsKS_p_max):
                rcvRateKS_p_max = rcvRateKS_p
                rcvRateKS_d_max = rcvRateKS_d
                sumMsgsKS_p_max = sumMsgsKS_p
                sumMsgsKS_d_max = sumMsgsKS_d
                sumMalMsgsKS_p_max = sumMalMsgsKS_p
                sumMalMsgsKS_d_max = sumMalMsgsKS_d
                train_maxVal = set1
                test_maxVal = set2

    with open(attack+"_train.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerows(train_maxVal)

    with open(attack+"_test.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerows(test_maxVal)

    with open(attack+"_p_d_values.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["rcvRate",rcvRateKS_p_max,rcvRateKS_d_max])
        writer.writerow(["sumMsgs",sumMsgsKS_p_max,sumMsgsKS_d_max])
        writer.writerow(["sumMalMsgsKS",sumMalMsgsKS_p_max,sumMalMsgsKS_d_max])


