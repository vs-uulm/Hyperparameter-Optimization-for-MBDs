#!/bin/bash


python3 csv_table.py -groundTruthFile ../../../../veremi/const_pos_extended/traceGroundTruthJSON-7.json -path ../../../../veremi/const_pos_extended/ -attack const_pos -gt position
python3 csv_table.py -groundTruthFile ../../../../veremi/random_speed_extended/traceGroundTruthJSON-7.json -path ../../../../veremi/random_speed_extended/ -attack rand_speed -gt speed
python3 csv_table.py -groundTruthFile ../../../../veremi/sudden_stop_extended/traceGroundTruthJSON-7.json -path ../../../../veremi/sudden_stop_extended/ -attack sudden_stop -gt position

python3 train_test_dataset.py -file rand_speed_output.csv -attack rand_speed -division 0.3
python3 train_test_dataset.py -file const_pos_output.csv -attack const_pos -division 0.3
python3 train_test_dataset.py -file sudden_stop_output.csv -attack sudden_stop -division 0.3


python3 copy_files.py -csvFile const_pos_train.csv -sourceDirectory ../../../../veremi/const_pos_extended -destinationDirectory ../train/const_pos/
python3 copy_files.py -csvFile rand_speed_train.csv -sourceDirectory ../../../../veremi/random_speed_extended -destinationDirectory ../train/rand_speed/
python3 copy_files.py -csvFile sudden_stop_train.csv -sourceDirectory ../../../../veremi/sudden_stop_extended -destinationDirectory ../train/sudden_stop/

python3 copy_files.py -csvFile const_pos_test.csv -sourceDirectory ../../../../veremi/const_pos_extended -destinationDirectory ../test/const_pos/
python3 copy_files.py -csvFile rand_speed_train.csv -sourceDirectory ../../../../veremi/random_speed_extended -destinationDirectory ../test/rand_speed/
python3 copy_files.py -csvFile sudden_stop_test.csv -sourceDirectory ../../../../veremi/sudden_stop_extended -destinationDirectory ../test/sudden_stop/

