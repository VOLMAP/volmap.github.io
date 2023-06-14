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
#ifndef ENDPOINTS_AND_SEPARATRICES_H
#define ENDPOINTS_AND_SEPARATRICES_H

#include <cinolib/meshes/tetmesh.h>

enum Domain {CUBE, PYRAMID, TETRAHEDRON, OCTAHEDRON, STAR, CUSTOM};

static std::vector<std::pair<uint,uint>> cube_arcs
{
    {0,1}, {1,2}, {2,3}, {0,3}, {0,4}, {1,5}, {2,6}, {3,7}, {4,5}, {5,6}, {6,7}, {4,7}
};

static std::vector<std::pair<uint,uint>> pyramid_arcs
{
    {0,1}, {1,2}, {2,3}, {0,3}, {4,0}, {4,1}, {4,2}, {4,3}
};

static std::vector<std::pair<uint,uint>> tetrahedron_arcs
{
    {0,1}, {1,2}, {0,2}, {3,0}, {3,1}, {3,2}
};

static std::vector<std::pair<uint,uint>> octahedron_arcs
{
    {0,1}, {1,2}, {2,3}, {3,0}, {4,0}, {4,1}, {4,2}, {4,3}, {5,0}, {5,1}, {5,2}, {5,3}
};


// if mask[v] = true, path cannot pass through it
double dijkstraOnlySurface(const cinolib::Tetmesh<> &m, uint source, uint dest,
                         const std::vector<bool> &v_mask, std::vector<uint> &path);

void updateVertexMask(const std::vector<uint> &path, std::vector<bool> &v_mask);

// flag edges in separatrices and split edges in proximity of ednpoints if required
void updateMeshConnectivity(cinolib::Tetmesh<> &m, std::vector<bool> &v_mask,
                            const std::vector<uint> &path);

void computeSrfCorners(const cinolib::Tetmesh<> &m,  Domain dom,
                       std::vector<uint> &srf_corners,
                       std::vector<cinolib::vec3d> &custom_dom_nodes);

void computeSrfPaths(cinolib::Tetmesh<> &m, Domain dom,
                     std::vector<uint> &srf_corners,
                     std::vector<std::vector<uint>> &srf_paths,
                     std::vector<std::pair<uint,uint>> &custom_dom_arcs);

void loadFromFile(std::string &domain_file, const cinolib::Tetmesh<> &m,
                  std::vector<cinolib::vec3d> &custom_dom_nodes,
                  std::vector<std::pair<uint,uint>> &custom_dom_arcs,
                  std::vector<cinolib::vec3d> &custom_map_corners);


void stringSplit(std::string &str, std::vector<std::string> &str_vec);

#endif //ENDPOINTS_AND_SEPARATRICES_H
