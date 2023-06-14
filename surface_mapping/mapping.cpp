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
#include "mapping.h"
#include <cinolib/sampling.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/export_surface.h>

void mapSrfCorners(cinolib::Trimesh<> &m_srf, Domain dom,
                   const std::unordered_map<uint, uint> &vol2srf,
                   const std::vector<uint> &srf_corners,
                   std::vector<cinolib::vec3d> &custom_map_corners)
{
    std::vector<cinolib::vec3d> *indexes;

    switch(dom)
    {
        case ::CUBE:          indexes = &cube_map_corners; break;
        case ::PYRAMID:       indexes = &pyramid_map_corners; break;
        case ::TETRAHEDRON:   indexes = &tetrahedron_map_corners; break;
        case ::OCTAHEDRON:    indexes = &octahedron_map_corners; break;
        case ::STAR:          indexes = &star_map_corners; break;
        case ::CUSTOM:        indexes = &custom_map_corners; break;
    }

    for(uint i = 0; i < srf_corners.size(); i++)
    {
        uint ep = srf_corners[i];
        uint srf_id = vol2srf.at(ep);
        m_srf.vert(srf_id) = indexes->at(i);
    }
}

//=============================================================================

void mapSrfPaths(cinolib::Trimesh<> &m_srf,
                 const std::unordered_map<uint, uint> &vol2srf,
                 const std::vector<std::vector<uint>> &srf_paths)
{
    using namespace cinolib;

    vec3d v_start, v_stop;
    std::vector<vec3d> samples;

    for(auto &s : srf_paths)
    {
        v_start = m_srf.vert(vol2srf.at(s.front()));
        v_stop = m_srf.vert(vol2srf.at(s.back()));
        samples = cinolib::sample_within_interval(v_start, v_stop, s.size());

        assert(samples.front() == v_start);
        assert(samples.back() == v_stop);

        for(uint i = 1; i < s.size()-1; i++)
        {
            uint srf_id = vol2srf.at(s[i]);
            m_srf.vert(srf_id) = samples[i];
        }

        for(uint i = 0; i < s.size()-1; i++)
        {
            uint v0id = vol2srf.at(s[i]);
            uint v1id = vol2srf.at(s[i+1]);
            uint eid = m_srf.edge_id(v0id, v1id);
            m_srf.edge_data(eid).flags[MARKED] = true;
            m_srf.vert_data(v0id).flags[MARKED] = true;
            m_srf.vert_data(v1id).flags[MARKED] = true;
        }
    }
}

//=============================================================================

void mapSrfFaces(cinolib::Trimesh<> &m_srf, int laplacian_mode)
{
    using namespace cinolib;

    std::map<uint,vec3d> boundary_conditions;
    std::vector<vec3d> new_vertices;

    for(uint vi = 0; vi < m_srf.num_verts(); vi++)
    {
        if(m_srf.vert_data(vi).flags[MARKED])
            boundary_conditions[vi] = m_srf.vert(vi);
    }

    new_vertices = harmonic_map_3d(m_srf, boundary_conditions, 1, laplacian_mode, SIMPLICIAL_LLT);
    assert(new_vertices.size() == m_srf.num_verts());
    m_srf.vector_verts() = new_vertices;
}

//=============================================================================

void resetState(std::vector<uint> &endpoints, std::vector<std::vector<uint>> &separatrices,
                std::unordered_map<uint,uint> &vol2srf)
{
    endpoints.clear();
    separatrices.clear();
    vol2srf.clear();
}

//=============================================================================

void saveSrfMappingOnFile(const std::string &file_out,
                          const cinolib::Trimesh<> &m_srf,
                          const std::unordered_map<uint, uint> &srf2vol)
{
    FILE *f = fopen(file_out.c_str(), "w");

    for(uint vi = 0; vi < m_srf.num_verts(); vi++)
    {
        uint vol_id = srf2vol.at(vi);
        fprintf(f,"%d %lf %lf %lf\n", vol_id, m_srf.vert(vi).x(), m_srf.vert(vi).y(), m_srf.vert(vi).z());
    }

    fclose(f);
}

//=============================================================================

void saveUpdatedInputOnFile(const std::string &file_in, const cinolib::Tetmesh<> &m_vol)
{
    std::string file_out = file_in.substr(0, file_in.length()-5) + "_in.mesh";
    m_vol.save(file_out.c_str());
}
