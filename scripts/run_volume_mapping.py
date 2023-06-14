# Given a folder "vol_in" containing the input tetmeshes, and a folder "srf_maps" containing the surface mappings computed with our surface mapping tool,
# this script creates a third folder called "vol_maps" containing the volumetric mappings created with the mapping algorithm you want to validate.
# Before using this script, customize the "exec_volmap" variable with the name of your executable file.

import os

exec_volmap = "mapping_vol" # customize this variable as explained before

curr_dir_path = './'
srf_map_folder = "srf_maps"
vol_map_folder = "vol_maps"

os.system("mkdir ../" + vol_map_folder)

files = os.listdir(curr_dir_path)

for f_in in files:
    if '.mesh' in f_in:
        base_name = f_in[0:-5]

        srf_map_file = "../" + srf_map_folder + "/" + base_name + "_smap.txt"
        vol_map_file = "../" + vol_map_folder + "/" + base_name + "_vmap.mesh"

        if os.path.exists(srf_map_file):
            os.system("./" + exec_volmap + " " + f_in + " " + srf_map_file + " " + vol_map_file)