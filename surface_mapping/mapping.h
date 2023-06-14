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

#ifndef MAPPING_H
#define MAPPING_H

#include <cinolib/meshes/trimesh.h>
#include "endpoints_and_separatrices.h"

static std::vector<cinolib::vec3d> cube_map_corners
{
    cinolib::vec3d(0.0, 0.0, 0.0),       // v0
    cinolib::vec3d(0.0, 0.0, 100.0),     // v1
    cinolib::vec3d(100.0, 0.0, 100.0),   // v2
    cinolib::vec3d(100.0, 0.0, 0.0),     // v3
    cinolib::vec3d(0.0, 100.0, 0.0),     // v4
    cinolib::vec3d(0.0, 100.0, 100.0),   // v5
    cinolib::vec3d(100.0, 100.0, 100.0), // v6
    cinolib::vec3d(100.0, 100.0, 0.0)    // v7
};

static std::vector<cinolib::vec3d> pyramid_map_corners
{
    cinolib::vec3d(0.0, 0.0, 0.0),       // v0
    cinolib::vec3d(0.0, 0.0, 100.0),     // v1
    cinolib::vec3d(100.0, 0.0, 100.0),   // v2
    cinolib::vec3d(100.0, 0.0, 0.0),     // v3
    cinolib::vec3d(50.0, 100.0, 50.0)    // v4
};

static std::vector<cinolib::vec3d> tetrahedron_map_corners
{
    cinolib::vec3d(0.0, 0.0, 0.0),     // v0
    cinolib::vec3d(0.0, 0.0, 100.0),   // v1
    cinolib::vec3d(100.0, 0.0, 50.0),  // v2
    cinolib::vec3d(33.3, 50.0, 33.3)   // v3
};

static std::vector<cinolib::vec3d> octahedron_map_corners
{
        cinolib::vec3d(0.0, 0.0, 0.0),       // v0
        cinolib::vec3d(0.0, 0.0, 100.0),     // v1
        cinolib::vec3d(100.0, 0.0, 100.0),   // v2
        cinolib::vec3d(100.0, 0.0, 0.0),     // v3
        cinolib::vec3d(50.0, 50.0, 50.0),   // v4
        cinolib::vec3d(50.0, -50.0, 50.0)   // v5
};

static std::vector<cinolib::vec3d> star_map_corners
{
        cinolib::vec3d(0.0, 0.0, 0.0),       // v0
        cinolib::vec3d(0.0, 0.0, 100.0),     // v1
        cinolib::vec3d(100.0, 0.0, 100.0),   // v2
        cinolib::vec3d(100.0, 0.0, 0.0),     // v3
        cinolib::vec3d(0.0, 100.0, 0.0),     // v4
        cinolib::vec3d(0.0, 100.0, 100.0),   // v5
        cinolib::vec3d(100.0, 100.0, 100.0), // v6
        cinolib::vec3d(100.0, 100.0, 0.0),   // v7
        cinolib::vec3d(-100.0, 50.0, 50.0),  // v8
        cinolib::vec3d(50.0, -100.0, 50.0),  // v9
        cinolib::vec3d(50.0, 50.0, 200.0),   // v10
        cinolib::vec3d(50.0, 200.0, 50.0),   // v11
        cinolib::vec3d(50.0, 50.0, -100.0),  // v12
        cinolib::vec3d(200.0, 50.0, 50)      // v13
};

void mapSrfCorners(cinolib::Trimesh<> &m_srf, Domain dom,
                   const std::unordered_map<uint, uint> &vol2srf,
                   const std::vector<uint> &srf_corners,
                   std::vector<cinolib::vec3d> &custom_map_corners);

void mapSrfPaths(cinolib::Trimesh<> &m_srf,
                 const std::unordered_map<uint, uint> &vol2srf,
                 const std::vector<std::vector<uint>> &srf_paths);

void mapSrfFaces(cinolib::Trimesh<> &m_srf, int laplacian_mode);

void resetState(std::vector<uint> &endpoints, std::vector<std::vector<uint>> &separatrices,
                std::unordered_map<uint,uint> &vol2srf);

void saveSrfMappingOnFile(const std::string &file_out,
                          const cinolib::Trimesh<> &m_srf,
                          const std::unordered_map<uint, uint> &srf2vol);

void saveUpdatedInputOnFile(const std::string &file_in, const cinolib::Tetmesh<> &m_vol);

#endif //MAPPING_H
