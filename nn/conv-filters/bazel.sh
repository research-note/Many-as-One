#!/bin/bash
# clean the current build
bazel build --config=clang_config //src:main

# make image matrices and store them
python3 make_mats.py img_mats/out.dat;

# run the filter program
./bazel-bin/src/main img_mats/out.dat img_mats/filter_out.dat < filter.txt

# run python program to generate images from matrix
# output written as 0.bmp, 1.bmp ...
python3 load_img.py img_mats/filter_out.dat out_mats
