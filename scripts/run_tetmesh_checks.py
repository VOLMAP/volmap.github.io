# This script runs the input check in all the ".mesh" files inside the current folder and saves the check's result in the input-checks.csv file.

import os

curr_dir_path = './'

files = os.listdir(curr_dir_path)

for f_in in files:
    if '.mesh' in f_in:
        os.system("./tetmesh_checks " + f_in + " input-checks.csv")