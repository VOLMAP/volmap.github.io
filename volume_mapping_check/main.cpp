#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/volume_mesh_controls.h>
#include "rationals_io.h"

//print colors
#define BLD "\033[1m"
#define RST "\033[0m"

using namespace cinolib;

//print usage
void usage() {
    std::cout << BLD << "Standard usage:" << RST << std::endl;
    std::cout << "./volume_mapping_check <input.mesh> <output.mesh> <input.txt> <output.txt>" << std::endl;
    std::cout << BLD << "Alternative usage 1 (no input.txt):" << RST << std::endl;
    std::cout << "./volume_mapping_check <input.mesh> <output.mesh> <output.txt>" << std::endl;
    std::cout << BLD << "Alternative usage 2 (no rationals at all):" << RST << std::endl;
    std::cout << "./volume_mapping_check <input.mesh> <output.mesh>" << std::endl;
    std::cout << BLD << "Where:" << RST << std::endl;
    std::cout << "- intput.mesh and output.mesh are the input and output meshes" << std::endl;
    std::cout << "- input.txt and output.txt are the rationals coordinate files where every 3 lines we have x, y, z of a vertex" << std::endl;
}
//map checks
std::pair<int, int> inputCheck(Tetmesh<> &m);
std::pair<int, int> inputCheck(Tetmesh<> &m, std::vector<CGAL_Q> &rt);
std::pair<int, int> mapCheck(Tetmesh<> &input, Tetmesh<> &output, std::vector<CGAL_Q> &input_rt, std::vector<CGAL_Q> &output_rt);

int main(int argc, char **argv) {

    //input check
    if(argc < 3 || argc > 5) {
        usage();
        exit(-1);
    }

    //parse the arguments
    std::string input_path = argv[1];
    std::string output_path = argv[2];
    std::string input_rt_path, output_rt_path;
    if(argc == 5) input_rt_path = argv[3];
    if(argc >= 4) output_rt_path = argv[argc-1];

    //load the meshes
    DrawableTetmesh<> input(input_path.c_str());
    DrawableTetmesh<> output(output_path.c_str());
    //load the rationals
    std::vector<CGAL_Q> input_rt, output_rt;
    if(argc == 5) load_RT(input_rt, input_rt_path);
    if(argc >= 4) load_RT(output_rt, output_rt_path);

    //check if the input is valid
    std::pair<int, int> inputNV = argc == 5 ? inputCheck(input, input_rt) : inputCheck(input);
    if(inputNV.first + inputNV.second != 0) {
        std::cout << "The input model is not valid [" << inputNV.first+inputNV.second << " non valid tets]" << std::endl;
        exit(-1);
    }

    //check if the map is valid
    std::pair<int, int> mapNV = mapCheck(input, output, input_rt, output_rt);
    if(argc >= 4 && mapNV.second == 0)  std::cout << "The map is valid [rationals check]" << std::endl;
    else if(argc >= 4)                  std::cout << "The map is not valid [" << mapNV.second << " elements not valid -- rationals check]" << std::endl;
    else if(mapNV.first == 0)           std::cout << "The map is valid [floating point check]" << std::endl;
    else                                std::cout << "The map is not valid [" << mapNV.first << " elements not valid -- floating point check]" << std::endl;

    return 0;
}

/** Check if the orientation of the volume sign of all the tetrahedra in the mesh is coherent.
 ** The reference sign is the one of the first tetrahedron. The check is done with the floating point arithmetic.
 **
 ** @param m the tetrahedral mesh
 ** @return a pair with the number of flipped tetrahedra (first) and the number of collapsed tetrahedra (second)
 **/
std::pair<int, int> inputCheck(Tetmesh<> &m) {
    int col = 0;
    int flip = 0;
    std::vector<vec3d> poly;
    double orient, reference;

    //getting the reference sign
    poly = m.poly_verts(0);
    //check the orientation
    reference = orient3d(poly[0], poly[1], poly[2], poly[3]);

    for(uint pid=0; pid<m.num_polys(); pid++) {
        poly = m.poly_verts(pid);
        //check the orientation
        orient = orient3d(poly[0], poly[1], poly[2], poly[3]);
        if(orient * reference < 0) flip++;
        else if(orient == 0) col++;
    }

    return std::make_pair(flip, col);
}


/** Check if the orientation of the volume sign of all the tetrahedra in the mesh is coherent.
 ** The reference sign is the one of the first tetrahedron. The check is done with rational arithmetic.
 **
 ** @param m the tetrahedral mesh
 ** @param rt the rational coordinates of the vertices
 ** @return a pair with the number of flipped tetrahedra (first) and the number of collapsed tetrahedra (second)
 **/
std::pair<int, int> inputCheck(Tetmesh<> &m, std::vector<CGAL_Q> &rt) {

    if(rt.empty()) return std::make_pair(-1, -1);

    int col = 0;
    int flip = 0;
    std::vector<uint> poly;
    CGAL_Q *poly_rt[4];
    CGAL_Q orient, reference;

    //getting the reference sign
    poly = m.poly_verts_id(0);
    //get the rationals of the poly
    poly_rt[0] = &rt[poly[0]*3];
    poly_rt[1] = &rt[poly[1]*3];
    poly_rt[2] = &rt[poly[2]*3];
    poly_rt[3] = &rt[poly[3]*3];
    //check the orientation
    reference = orient3d(poly_rt[0], poly_rt[1], poly_rt[2], poly_rt[3]);

    for(uint pid=0; pid<m.num_polys(); pid++) {
        poly = m.poly_verts_id(pid);
        //get the rationals of the poly
        poly_rt[0] = &rt[poly[0]*3];
        poly_rt[1] = &rt[poly[1]*3];
        poly_rt[2] = &rt[poly[2]*3];
        poly_rt[3] = &rt[poly[3]*3];
        //check the orientation
        orient = orient3d(poly_rt[0], poly_rt[1], poly_rt[2], poly_rt[3]);
        if(orient * reference < 0) flip++;
        else if(orient == 0) col++;
    }

    return std::make_pair(flip, col);
}

/** Check if the orientation of the volume sign of all the tetrahedra in the input mesh is coherent with the output mesh.
 ** The check is done with floating point and rational arithmetic.
 ** If the output rationals are not provided, the check is done only with floating point.
 ** If the input rationals are not provided, the check is done between input floating point and output rationals.
 **
 ** @param input the input tetrahedral mesh
 ** @param output the output tetrahedral mesh
 ** @param input_rt the rational coordinates of the input vertices
 ** @param output_rt the rational coordinates of the output vertices
 ** @return a pair with the number of flipped tetrahedra with floating point (first) and with rationals (second)
 **/
std::pair<int, int> mapCheck(Tetmesh<> &input, Tetmesh<> &output, std::vector<CGAL_Q> &input_rt, std::vector<CGAL_Q> &output_rt) {

    if(input.num_verts() != output.num_verts()) return std::make_pair(-1, -1);
    if(input.num_polys() != output.num_polys()) return std::make_pair(-1, -1);

    std::pair<int, int> flips(0, 0);
    double orient_fp_input, orient_fp_output;
    CGAL_Q orient_rt_input, orient_rt_output;
    std::vector<uint> poly;
    for(int pid = 0; pid < input.num_polys(); pid++) {
        //get the vids
        poly = input.poly_verts_id(pid);
        if(output.poly_id_from_vids(poly) == -1) return std::make_pair(-1, -1);
        //get orient in floating point
        orient_fp_input = orient3d(input.vert(poly[0]), input.vert(poly[1]), input.vert(poly[2]), input.vert(poly[3]));
        orient_fp_output = orient3d(output.vert(poly[0]), output.vert(poly[1]), output.vert(poly[2]), output.vert(poly[3]));
        if(orient_fp_input * orient_fp_output <= 0) flips.first++;
        //get orient in rational (need at least output_rt)
        if(output_rt.empty()) {
            //if there are no input rationals, the check is done between input floating point and output rationals
            if(!input_rt.empty())orient_rt_input = orient3d(&input_rt[poly[0] * 3], &input_rt[poly[1] * 3], &input_rt[poly[2] * 3],&input_rt[poly[3] * 3]);
            else orient_rt_input = orient_fp_input;
            orient_rt_output = orient3d(&output_rt[poly[0] * 3], &output_rt[poly[1] * 3], &output_rt[poly[2] * 3],&output_rt[poly[3] * 3]);
            if(orient_rt_input * orient_rt_output <= 0) flips.second++;
        }
    }
    if(output_rt.empty()) flips.second = -1;

    return flips;
}