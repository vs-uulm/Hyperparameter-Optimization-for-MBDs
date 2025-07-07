#!/bin/bash

python3 run-file.py -outputFile output/const_pos_test_weights.json -attackType 1
python3 run-file.py -outputFile output/rand_speed_test_weights.json -attackType 2
python3 run-file.py -outputFile output/sudden_stop_test_weights.json -attackType 3
python3 run-file.py -outputFile output/mixed_test_weights.json -attackType 4

jupyter nbconvert --execute --to notebook plot_p_r_test.ipynb

