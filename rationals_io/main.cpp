/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2024 Federico Meloni, Gianmarco Cherchi, Marco Livesu                   *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                *
 *                                                                                       *
 * Authors:                                                                              *
 *      Federico Meloni (federico.meloni3@unica.it)                                      *
 *      https://www.federicheddu.github.io                                               *
 *                                                                                       *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://www.gianmarcocherchi.com                                                 *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 * ***************************************************************************************/

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
