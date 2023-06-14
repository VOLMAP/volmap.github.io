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

#include <cinolib/meshes/trimesh.h>
#include <cinolib/meshes/tetmesh.h>
#include <cinolib/tetgen_wrap.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "Command Syntax Error! Correct syntax: ./tetmesh_generation <srf_model.obj>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    cinolib::Trimesh<> m_srf = cinolib::Trimesh<>(filename.c_str());
    cinolib::Tetmesh<> m_vol;

    // Tetmesh computation
    cinolib::tetgen_wrap(m_srf, "YqQ", m_vol);

    std::string fileout = filename.substr(0, filename.length() -3) + "mesh";
    m_vol.save(fileout.c_str());
    std::cout << "File " << fileout << " saved" << std::endl;

    return EXIT_SUCCESS;
}
