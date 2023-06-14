/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2023 Gianmarco Cherchi, Marco Livesu                                    *
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
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://www.gianmarcocherchi.com                                                 *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 * ***************************************************************************************/

#undef NDEBUG

#include <cinolib/meshes/tetmesh.h>
#include <ios>
#include <fstream>
#include "functions.h"

using namespace cinolib;

int main(int argc, char **argv)
{
    Data s;
    s.upper_bound = 100.0;

    std::string orig_filename;
    std::string map_filename;
    std::string log_filename;
    bool on_log = false;

    if(argc < 3 || argc > 5)
    {
        std::cout << "Command Syntax Error! Correct syntax: " << std::endl;
        std::cout << "./metrics <input_model.mesh> <vol_mapping.mesh>" << std::endl;
        std::cout << "./metrics <input_model.mesh> <vol_mapping.mesh> <upper_bound>" << std::endl;
        std::cout << "./metrics <input_model.mesh> <vol_mapping.mesh> <upper_bound> <log_file.csv>" << std::endl;
        return EXIT_FAILURE;
    }

    orig_filename = argv[1];
    map_filename = argv[2];

    if(argc >= 4)   
        s.upper_bound = atoi(argv[3]);
    
    if(argc == 5)
    {
        log_filename = argv[4];
        on_log = true;
    }

    read_MESH(orig_filename.c_str(), s.orig_verts, s.orig_polys);
    read_MESH(map_filename.c_str(), s.map_verts, s.map_polys);
    assert(s.orig_verts.size() == s.map_verts.size());
    assert(s.orig_polys.size() == s.map_polys.size());

    computeOrientAndVolume(s);
    computeInvertedTets(s);

    computeDistortionEnergy(s, DistEnergy::CONFORMAL, 1.0, s.upper_bound, s.conformal);
    computeDistortionEnergy(s, DistEnergy::DIRICHLET, 3.0, s.upper_bound, s.dirichlet);
    computeDistortionEnergy(s, DistEnergy::SYMMETRIC_DIRICHLET, 6.0, s.upper_bound, s.symmetric_dirichlet);
    computeDistortionEnergy(s, DistEnergy::ARAP, 0.0, s.upper_bound, s.arap);
    computeDistortionEnergy(s, DistEnergy::MIPS3D, 1.0, s.upper_bound, s.mips3d);
    computeTetRadiusRatio(s);

    if(!on_log) printValuesOnScreen(s);
    else        printValuesOnFile(map_filename, log_filename, s);

    return EXIT_SUCCESS;
}
