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
#include <cinolib/string_utilities.h>

using namespace cinolib;

int main(int argc, char **argv)
{
    std::string filename, fileout;

    if(argc == 2)
        filename = argv[1];
    else
    {
        std::cout << "Command Syntax Error! Correct syntax: ./mesh_converter <file.mesh>|<file.vtk>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string basename = get_file_path(filename, true);
    std::string ext = get_file_extension(filename);

    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> tets;

    if(std::strcmp(ext.c_str(), "mesh") == 0)
    {
        fileout = basename + ".vtk";
        read_MESH(filename.c_str(), verts, tets);
        write_VTK(fileout.c_str(), verts, tets);
    }
    else if(std::strcmp(ext.c_str(), "vtk") == 0)
    {
        fileout = basename + ".mesh";
        read_VTK(filename.c_str(), verts, tets);
        write_MESH(fileout.c_str(), verts, tets);
    }
    else
    {
        std::cout << "ERROR: file extension not supported!\n";
        return EXIT_FAILURE;
    }

    std::cout << "file " << fileout << " saved" << std::endl;
    

    return EXIT_SUCCESS;
}
