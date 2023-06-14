# This script runs the surface mapping tool on all the ".mesh" files in the current folder.
# Put this script in the folder containing the ".mesh" input previously checked with our tetmesh check tool. 
# The output surface mappings are saved in the "srf_maps" subfolder, while the modified input meshes are saved in the "vol_in" folder.
# Before running this script, customize the "domain" variable with one of the following values: CUBE, PYRAMID, TETRAHEDRON, OCTAHEDRON or STAR.
# Customize the script if you want to use a custom domain.

import os

domain = "CUBE" # customize this variable as explained before

curr_dir_path = './'
input_subfolder = "vol_in"
maps_subfolder = "srf_maps"

files = os.listdir(curr_dir_path)

os.system("mkdir " + maps_subfolder)
os.system("mkdir " + input_subfolder)

for f_in in files:
    if '.mesh' in f_in:

        base_name = f_in[0:-5]
        file_out = base_name + "_smap.txt"

        os.system("./surface_mapping " + domain + " " + f_in + " " + maps_subfolder + "/" + file_out)

        mesh_in = base_name + "_in.mesh"
        if os.path.exists(mesh_in):
            os.system("mv " + mesh_in + " " + input_subfolder + "/" + f_in)


        

