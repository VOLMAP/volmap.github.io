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
#include "endpoints_and_separatrices.h"
#include "cinolib/octree.h"
#include <fstream>
#include <string>


// if mask[v] = true, path cannot pass through it
double dijkstraOnlySurface(const cinolib::Tetmesh<> &m, uint source, uint dest,
                         const std::vector<bool> &mask, std::vector<uint> &path)
{
    const double inf_double = std::numeric_limits<double>::infinity();

    path.clear();
    assert(mask.size() == m.num_verts());

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = (int)vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            if(mask.at(nbr) && nbr!=dest) continue;

            uint eid = m.edge_id(vid, nbr);
            if(!m.edge_is_on_srf(eid)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = (int)vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }

    // there exists no path with the given mask constraints
    path.clear();
    return 0.0;
}

//=============================================================================

void updateVertexMask(const std::vector<uint> &path, std::vector<bool> &v_mask)
{
    for(uint p : path)
        v_mask[p] = true;
}

//=============================================================================

void updateMeshConnectivity(cinolib::Tetmesh<> &m, std::vector<bool> &v_mask,
                            const std::vector<uint> &path)
{
    using namespace cinolib;

    std::vector<std::pair<uint,uint>> edges_to_split;

    // marking edges involved in the new separatrice
    for(uint i = 0; i < path.size()-1; i++)
    {
        uint eid = m.edge_id(path[i], path[i+1]);
        m.edge_data(eid).flags[MARKED] = true;
    }

    for(uint ei = 0; ei < m.num_edges(); ei++)
    {
        if(!m.edge_is_on_srf(ei)) continue;

        uint v0 = m.edge_vert_id(ei, 0);
        uint v1 = m.edge_vert_id(ei, 1);
        if(v_mask[v0] && v_mask[v1] && !m.edge_data(ei).flags[MARKED]) //edge split required
            edges_to_split.emplace_back(v0, v1);
    }

    for(auto &e : edges_to_split)
    {
        int eid = m.edge_id(e.first, e.second);
        assert(eid >= 0 && "edge id not found!");
        m.edge_split(eid);
        v_mask.push_back(false);
    }
}

//=============================================================================

void computeSrfCorners(const cinolib::Tetmesh<> &m,  Domain dom,
                       std::vector<uint> &srf_corners,
                       std::vector<cinolib::vec3d> &custom_dom_nodes)
{
    using namespace cinolib;

    Octree o;

    for(uint vi = 0; vi < m.num_verts(); vi++)
    {
        if(m.vert_is_on_srf(vi))
            o.push_point(vi, m.vert(vi));
    }

    o.build();

    vec3d pos;
    double dist;
    std::vector<vec3d> nodes;

    switch(dom)
    {
        case ::CUBE:
        case ::STAR:
        {
            srf_corners.resize(8);
            nodes.resize(8);

            nodes[0] = m.bbox().min;
            nodes[6] = m.bbox().max;
            nodes[1] = vec3d(nodes[0].x(), nodes[0].y(), nodes[6].z());
            nodes[2] = vec3d(nodes[6].x(), nodes[0].y(), nodes[6].z());
            nodes[3] = vec3d(nodes[6].x(), nodes[0].y(), nodes[0].z());
            nodes[4] = vec3d(nodes[0].x(), nodes[6].y(), nodes[0].z());
            nodes[5] = vec3d(nodes[0].x(), nodes[6].y(), nodes[6].z());
            nodes[7] = vec3d(nodes[6].x(), nodes[6].y(), nodes[0].z());
        }
        break;

        case ::PYRAMID:
        {
            srf_corners.resize(5);
            nodes.resize(5);
            vec3d vmax = m.bbox().max;

            nodes[0] = m.bbox().min;
            nodes[1] = vec3d(nodes[0].x(), nodes[0].y(), vmax.z());
            nodes[2] = vec3d(vmax.x(), nodes[0].y(), vmax.z());
            nodes[3] = vec3d(vmax.x(), nodes[0].y(), nodes[0].z());
            nodes[4] = (nodes[0] + nodes[1] + nodes[2] + nodes[3]) / 4;
            nodes[4].y() = vmax.y();
        }
        break;

        case ::TETRAHEDRON:
        {
            srf_corners.resize(4);
            nodes.resize(4);
            vec3d vmax = m.bbox().max;

            nodes[0] = m.bbox().min;
            nodes[1] = vec3d(nodes[0].x(), nodes[0].y(), vmax.z());
            nodes[2] = (nodes[0] + nodes[1]) / 2;
            nodes[2].x() = vmax.x();
            nodes[3] = (nodes[0] + nodes[1] + nodes[2]) / 3;
            nodes[3].y() = vmax.y();
        }
        break;

        case ::OCTAHEDRON:
        {
            srf_corners.resize(6);
            nodes.resize(6);
            vec3d vmax = m.bbox().max;
            vec3d vmin = m.bbox().min;
            double middle_y = vmin.y() + std::abs(vmax.y() - vmin.y()) /2;

            nodes[0] = vec3d(vmin.x(), middle_y, vmin.z());
            nodes[1] = vec3d(vmin.x(), middle_y, vmax.z());
            nodes[2] = vec3d(vmax.x(), middle_y, vmax.z());
            nodes[3] = vec3d(vmax.x(), middle_y, vmin.z());
            nodes[4] = nodes[5] = (nodes[0] + nodes[1] + nodes[2] + nodes[3]) / 4;
            nodes[4].y() = vmax.y();
            nodes[5].y() = vmin.y();
        }
        break;

        case ::CUSTOM:
        {
            srf_corners.resize(custom_dom_nodes.size());
            nodes = custom_dom_nodes;
        }
        break;
    }

    for(uint i = 0; i < srf_corners.size(); i++)
        o.closest_point(nodes[i], srf_corners[i], pos, dist);
}

//=============================================================================

void computeSrfPaths(cinolib::Tetmesh<> &m, Domain dom,
                     std::vector<uint> &srf_corners,
                     std::vector<std::vector<uint>> &srf_paths,
                     std::vector<std::pair<uint,uint>> &custom_dom_arcs)
{
    std::vector<std::pair<uint, uint>> *indexes;
    double dist;

    // vertex mask: if v_mask[v] = true, path cannot pass through vert v
    std::vector<bool> v_mask(m.num_verts(), false);

    for(auto ep : srf_corners)
        v_mask[ep] = true;

    switch(dom)
    {
        case ::CUBE:
        {
            srf_paths.resize(12);
            indexes = &cube_arcs;
        }
        break;

        case ::PYRAMID:
        {
            srf_paths.resize(8);
            indexes = &pyramid_arcs;
        }
        break;

        case ::TETRAHEDRON:
        {
            srf_paths.resize(6);
            indexes = &tetrahedron_arcs;
        }
        break;

        case ::OCTAHEDRON:
        {
            srf_paths.resize(12);
            indexes = &octahedron_arcs;
        }
        break;

        case ::STAR:
        {
            srf_paths.resize(12);
            indexes = &cube_arcs; // to add diagonals
        }
        break;

        case ::CUSTOM:
        {
            srf_paths.resize(custom_dom_arcs.size());
            indexes = &custom_dom_arcs;
        }
        break;
    }

    for(uint i = 0; i < srf_paths.size(); i++)
    {
        uint v0 = indexes->at(i).first;
        uint v1 = indexes->at(i).second;
        dist = dijkstraOnlySurface(m, srf_corners[v0], srf_corners[v1], v_mask, srf_paths[i]);
        assert(dist > 0 && "computing path failed!");
        updateVertexMask(srf_paths[i], v_mask);
        updateMeshConnectivity(m, v_mask, srf_paths[i]);
    }

    // ONLY for the STAR case: we construct the srf_paths starting from the cube ones
    if(dom == STAR)
    {
        srf_paths.resize(36);
        std::vector<uint> tmp_sep;
        uint half_size;

        std::vector<std::pair<uint,uint>> diagonals = {{5,0}, {0,2}, {5,2}, {5,7}, {0,7}, {6,3}};

        uint curr_sep = 12;
        for(auto &p : diagonals)
        {
            dist = dijkstraOnlySurface(m, srf_corners[p.first], srf_corners[p.second], v_mask, tmp_sep);
            assert(dist > 0 && "computing path failed!");
            assert(tmp_sep.size() >= 3);
            half_size = tmp_sep.size() /2;
            for(uint i = 0; i <= half_size; i++)             srf_paths[curr_sep].push_back(tmp_sep[i]);
            for(uint i = half_size; i < tmp_sep.size(); i++) srf_paths[curr_sep + 1].push_back(tmp_sep[i]);
            srf_corners.push_back(tmp_sep[half_size]);
            updateVertexMask(tmp_sep, v_mask);
            updateMeshConnectivity(m, v_mask, tmp_sep);
            curr_sep +=2;
        }

        std::vector<std::pair<uint,uint>> missing_arcs = {{8, 1}, {8, 4}, {9, 1}, {9, 3}, {10, 1}, {10, 6}, {11, 4}, {11, 6}, {12, 3}, {12, 4}, {13, 2}, {13, 7}};

        for(auto &p : missing_arcs)
        {
            dist = dijkstraOnlySurface(m, srf_corners[p.first], srf_corners[p.second], v_mask, srf_paths[curr_sep]);
            assert(dist > 0 && "computing path failed!");
            updateVertexMask(srf_paths[curr_sep], v_mask);
            updateMeshConnectivity(m, v_mask, srf_paths[curr_sep]);
            curr_sep++;
        }
    }
}

//=============================================================================

void loadFromFile(std::string &domain_file, const cinolib::Tetmesh<> &m,
                  std::vector<cinolib::vec3d> &custom_dom_nodes,
                  std::vector<std::pair<uint,uint>> &custom_dom_arcs,
                  std::vector<cinolib::vec3d> &custom_map_corners)
{
    using namespace cinolib;
    std::ifstream f(domain_file);

    // bbox vertices
    std::vector<vec3d> bbox_verts(8);
    bbox_verts[0] = m.bbox().min;
    bbox_verts[6] = m.bbox().max;
    bbox_verts[1] = vec3d(bbox_verts[0].x(), bbox_verts[0].y(), bbox_verts[6].z());
    bbox_verts[2] = vec3d(bbox_verts[6].x(), bbox_verts[0].y(), bbox_verts[6].z());
    bbox_verts[3] = vec3d(bbox_verts[6].x(), bbox_verts[0].y(), bbox_verts[0].z());
    bbox_verts[4] = vec3d(bbox_verts[0].x(), bbox_verts[6].y(), bbox_verts[0].z());
    bbox_verts[5] = vec3d(bbox_verts[0].x(), bbox_verts[6].y(), bbox_verts[6].z());
    bbox_verts[7] = vec3d(bbox_verts[6].x(), bbox_verts[6].y(), bbox_verts[0].z());

    // load data from file
    if (f.is_open())
    {
        std::string line;
        while (std::getline(f, line))
        {
            std::vector<std::string> str_vec;
            stringSplit(line, str_vec);

            if(str_vec.empty())
                continue;

            else if(std::strcmp(str_vec[0].c_str(), "n") == 0) // corner
            {
                vec3d sum(0.0, 0.0, 0.0);
                for(uint i = 1; i < str_vec.size(); i++)
                    sum += bbox_verts[std::stoi(str_vec[i])];

                sum /= ((int)str_vec.size()-1);
                custom_dom_nodes.push_back(sum);
            }
            else if(std::strcmp(str_vec[0].c_str(), "nm") == 0) // corner mapping
            {
                custom_map_corners.emplace_back(std::stod(str_vec[1]),
                                                std::stod(str_vec[2]),
                                                std::stod(str_vec[3]));
            }
            else if(std::strcmp(str_vec[0].c_str(), "a") == 0) // arc
            {
                custom_dom_arcs.emplace_back(std::stoi(str_vec[1]),
                                             std::stoi(str_vec[2]));
            }
        }
        f.close();
    }

    assert(custom_dom_nodes.size() == custom_map_corners.size());
}

//=============================================================================

void stringSplit(std::string &str, std::vector<std::string> &str_vec)
{
    std::stringstream ss(str);
    char delim = ' ';
    std::string s;

    while (std::getline(ss, s, delim))
        str_vec.push_back(s);
}
