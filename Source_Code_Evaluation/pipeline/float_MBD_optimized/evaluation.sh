#!/bin/bash


# start training
python3 train.py

jupyter nbconvert --execute --to notebook plot_train.ipynb

python3 test.py -inputFile output/const_pos_train_pareto_front.json -outputFile output/const_pos_test_weights.json -attackType 1
python3 test.py -inputFile output/rand_speed_train_pareto_front.json -outputFile output/rand_speed_test_weights.json -attackType 2
python3 test.py -inputFile output/sudden_stop_train_pareto_front.json -outputFile output/sudden_stop_test_weights.json -attackType 3
python3 test.py -inputFile output/mixed_train_pareto_front.json -outputFile output/mixed_test_weights.json -attackType 4

jupyter nbconvert --execute --to notebook notebook plot_test.ipynb

