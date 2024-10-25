#include <cinolib/meshes/tetmesh.h>
#include "rationals_io.h"

using namespace cinolib;

int main(int argc, char **argv) {

    //input check (2 arguments)
    if(argc != 3) {
        std::cout << "Usage: ./rationals_io <model.mesh> <rationals.txt>" << std::endl;
        exit(-1);
    }

    //parse the arguments
    std::string model_path = argv[1];
    std::string rationals_path = argv[2];

    //load mesh and rationals
    Tetmesh<> m(model_path.c_str());
    std::vector<CGAL_Q> rationals;
    load_RT(rationals, rationals_path);

    /*
     * Remember that coordinates of vert with index i are stored in:
     * - rationals[3*i+0] ---> x
     * - rationals[3*i+1] ---> y
     * - rationals[3*i+2] ---> z
    */

    //DO YOUR STUFF HERE
    //...
    //...
    //...

    //save the results (in this example overwrites the input model and the rationals)
    m.save(model_path.c_str());
    save_RT(rationals, rationals_path);

    return 0;
}
