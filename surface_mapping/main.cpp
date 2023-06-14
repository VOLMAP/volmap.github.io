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

#include "endpoints_and_separatrices.h"
#include "mapping.h"

#include <cinolib/export_surface.h>


int main(int argc, char **argv)
{
    std::string file_in, file_out;
    std::string file_custom_dom;
    
    Domain dom;
    std::vector<uint> srf_corners;
    std::vector<std::vector<uint>> srf_paths;
    std::unordered_map<uint,uint> vol2srf, srf2vol;

    std::vector<cinolib::vec3d> custom_dom_nodes;       // only for CUSTOM domain
    std::vector<std::pair<uint,uint>> custom_dom_arcs;  // only for CUSTOM domain
    std::vector<cinolib::vec3d> custom_map_corners;     // only for CUSTOM domain

    if(argc < 4 || argc > 5)
    {
        std::cout << "Command Syntax Error! Correct syntax: " << std::endl;
        std::cout << "./surface_mapping <domain> <input.mesh> <srf_mapping.txt>" << std::endl;
        std::cout << "./surface_mapping CUSTOM <input.mesh> <srf_mapping.txt> <custom_domain.txt>" << std::endl;
        std::cout << "<domain> param can be: CUBE, PYRAMID, TETRAHEDRON, OCTAHEDRON, STAR or CUSTOM" << std::endl;
        return EXIT_FAILURE;
    }

    if     (std::strcmp(argv[1], "CUBE") == 0)        dom = ::CUBE;
    else if(std::strcmp(argv[1], "PYRAMID") == 0)     dom = ::PYRAMID;
    else if(std::strcmp(argv[1], "TETRAHEDRON") == 0) dom = ::TETRAHEDRON;
    else if(std::strcmp(argv[1], "OCTAHEDRON") == 0)  dom = ::OCTAHEDRON;
    else if(std::strcmp(argv[1], "STAR") == 0)        dom = ::STAR;
    else if(std::strcmp(argv[1], "CUSTOM") == 0)      dom = ::CUSTOM;
    else
    {
        std::cout << "Not supported <domain> option" << std::endl;
        return EXIT_FAILURE;
    }

    file_in  = argv[2];
    file_out = argv[3];

    if(dom == CUSTOM)
    {
        if(argc != 5)
        {
            std::cout << "Command Syntax Error: Custom domain file missing" << std::endl;
            return EXIT_FAILURE;
        }
        file_custom_dom = argv[4];
    }

    std::cout << "Model: " << file_in << std::endl;

    cinolib::Tetmesh<> m_vol(file_in.c_str());
    cinolib::Trimesh<> m_srf;

    // MAPPING pipeline start...

    if(dom == CUSTOM)
        loadFromFile(file_custom_dom, m_vol, custom_dom_nodes, custom_dom_arcs, custom_map_corners);
    
    computeSrfCorners(m_vol, dom, srf_corners, custom_dom_nodes);

    computeSrfPaths(m_vol, dom, srf_corners, srf_paths, custom_dom_arcs);
    
    cinolib::export_surface(m_vol, m_srf, vol2srf, srf2vol);
    mapSrfCorners(m_srf, dom, vol2srf, srf_corners, custom_map_corners);
    mapSrfPaths(m_srf, vol2srf, srf_paths);
    mapSrfFaces(m_srf, cinolib::UNIFORM);
    
    // ... MAPPING pipeline end.

    saveSrfMappingOnFile(file_out, m_srf, srf2vol);
    saveUpdatedInputOnFile(file_in, m_vol);
    
    std::cout << "Srf. mapping saved on file " << file_out << std::endl;
    return EXIT_SUCCESS;

}
