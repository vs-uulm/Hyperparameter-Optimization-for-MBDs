#!/bin/bash

python3 run-file.py -outputFile output/const_pos.json -attackType 1
python3 run-file.py -outputFile output/rand_speed.json -attackType 2
python3 run-file.py -outputFile output/sudden_stoz.json -attackType 3
python3 run-file.py -outputFile output/mixed.json -attackType 4

jupyter nbconvert --execute --to notebook plot_p_r_test.ipynb

