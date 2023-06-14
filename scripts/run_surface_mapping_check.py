# This script checks the validity of the surface mapping computed with our surface mapping check.
# Our run_surface_mapping.py script creates the folders "vol_in" and "srf_maps". 
# Put this script inside the "vol_in" folder.
# For each "<filename>.mesh" in the "vol_in" folder, the script checks the mapping in the "srf_maps" folder  
# (with name <filename>_smap.txt, as produced from our run_surface_mapping.py script).

import os

curr_dir_path = './'
maps_subfolder = "srf_maps"

files = os.listdir(curr_dir_path)

for f_in in files:
    if '.mesh' in f_in:

        base_name = f_in[0:-5]

        os.system("./surface_mapping_check " + f_in + " ../" + maps_subfolder + "/" + base_name + "_smap.txt")
        

