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
#include <cinolib/predicates.h>
#include <cinolib/export_surface.h>

using namespace cinolib;

struct Data
{
    Tetmesh<> m_vol;
    Tetmesh<> m_map;
    std::vector<vec3d> map_verts;
    std::map<uint,vec3d> srf_map;
    std::string filename;
    std::string bc_filename;
    vec3d centroid;
};

void read_srf_map(Data &s)
{
    s.centroid = vec3d(0,0,0);

    FILE *f = fopen(s.bc_filename.c_str(),"r");
    if(f)
    {
        while(!feof(f))
        {
            uint   vid;
            double x, y, z;
            fscanf(f,"%d %lf %lf %lf", &vid, &x, &y, &z);
            vec3d v(x, y, z);
            s.srf_map[vid] = v;
            s.centroid += v;
        }

        assert(s.srf_map.size() == s.m_vol.num_srf_verts());
        fclose(f);
    }
    else
        std::cout << s.bc_filename << " not found!" << std::endl;

    s.centroid = s.centroid / (int)s.srf_map.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    Data s; // algorithm state

    if(argc == 3)
    {
        s.filename = argv[1];
        s.bc_filename = argv[2];
    }
    else
    {
        std::cout << "Command Syntax Error! Correct syntax: ./surface_mapping_check <model.mesh> <srf_mapping.txt>" << std::endl;
        return EXIT_FAILURE;
    }

    s.m_vol = Tetmesh<>(s.filename.c_str());
    read_srf_map(s);

    std::cout << "Model: " << s.filename << std::endl;

    s.map_verts.resize(s.m_vol.num_verts());

    for(uint vi = 0; vi < s.m_vol.num_verts(); vi++)
    {
        auto f = s.srf_map.find(vi);
        if(f != s.srf_map.end())
            s.map_verts[vi] = f->second;
        else
            s.map_verts[vi] = s.centroid;
    }

    s.map_verts.push_back(s.centroid);
    uint c_id = s.map_verts.size()-1;

    // check polys orientation
    uint num_pos = 0, num_neg = 0, num_zero = 0;
    uint num_p = 0;

    for(uint fi = 0; fi < s.m_vol.num_faces(); fi++)
    {
        if(!s.m_vol.face_is_on_srf(fi)) continue;

        const auto &f = s.m_vol.face_verts_id(fi);

        double o = orient3d(s.map_verts.at(f[0]), s.map_verts.at(f[1]), s.map_verts.at(f[2]), s.map_verts.back());
        if(o < 0)       ++num_neg;
        else if(o > 0)  ++num_pos;
        else            ++num_zero;
        ++num_p;
    }

    if((num_pos == num_p) || (num_neg == num_p))
        std::cout << "Check result: SUCCESS" << std::endl;
    else 
        std::cout << "Check result: FAIL" << std::endl;    
    
    return EXIT_SUCCESS;
}
