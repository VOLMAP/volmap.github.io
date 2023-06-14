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
#include <cinolib/connected_components.h>
#include <cinolib/find_intersections.h>
#include <ios>
#include <fstream>

using namespace cinolib;

int main(int argc, char **argv)
{
    std::string filename;
    std::string log_filename;
    std::string fileout;
    bool on_log = false;

    bool check_orient = true;
    bool check_volumes = true;
    bool check_genus = true;
    bool check_conn_comp = true;
    bool check_srf_manif = true;
    bool check_bord_edges = true;
    bool check_self_inters = true;

    if(argc < 2 || argc > 3)
    {
        std::cout << "Command Syntax Error! Correct syntax: " << std::endl;
        std::cout << "./tetmesh_checks <file.mesh>" << std::endl;
        std::cout << "./tetmesh_checks <file.mesh> <log_file.csv>" << std::endl;
        return EXIT_FAILURE;
    }

    filename = argv[1];

    if(argc == 3) 
    {
        log_filename = argv[2];
        on_log = true;
    }
    
    std::vector<vec3d> verts;
    std::vector<std::vector<uint>> tets;
    read_MESH(filename.c_str(), verts, tets);

    std::cout << "Model: " << filename << std::endl;

    // check orientation
    const std::vector<uint> &tet = tets[0];
    double OR = orient3d(verts.at(tet[0]), verts.at(tet[1]), verts.at(tet[2]), verts.at(tet[3]));
    assert(OR != 0);

    if(OR < 0)
    {
        for(auto &t : tets)
            std::swap(t[0], t[1]);

        fileout = filename.substr(0, filename.length() -5) + "_inv.mesh";
        write_MESH(fileout.c_str(), verts, tets);
        check_orient = false;
    }

    // check volumes
    Tetmesh<> m(verts, tets);
    uint num_pos = 0, num_neg = 0, num_zero = 0;
    
    for(uint ti = 0; ti < m.num_polys(); ti++)
    {
        const auto &tet = m.poly_verts(ti);
        double o = orient3d(tet[0], tet[1], tet[2], tet[3]);

        if(o == 0)     num_zero++;
        else if(o < 0) num_neg++;
        else           num_pos++;
    }

    if(num_pos != m.num_polys())  check_volumes = false;   

    Trimesh<> m_srf;
    export_surface(m, m_srf);

    // check genus
    int genus = m_srf.genus();
    if(genus != 0) check_genus = false;

    // check connected component
    int num_cc = cinolib::connected_components(m_srf);
    if(num_cc != 1)  check_conn_comp = false;

    // check manifoldness
    if(!m_srf.mesh_is_manifold())  check_srf_manif = false;

    // check surface borders
    int num_srf_edges = 0;
    for(uint ei = 0; ei < m_srf.num_edges(); ei++)
        if(m_srf.edge_is_boundary(ei))
            ++num_srf_edges;
        
    if(num_srf_edges != 0)  check_bord_edges = false;

    // check surface self intersections
    std::set<ipair> intersections;
    find_intersections(m_srf, intersections);
    if(!intersections.empty())  check_self_inters = false;

    if(!on_log)
    {
        if(check_orient) std::cout << "Check Orientation:    PASSED" << std::endl;
        else             std::cout << "Check Orientation:    Inverted orientation, file " << fileout << " saved" << std::endl;

        if(check_volumes) std::cout << "Check Volume:\t      PASSED" << std::endl;
        else              std::cout << "Check Volume:\t      FAILED (num vol neg: " << num_neg << " , num vol null: " << num_zero << ")" << std::endl;

        if(check_genus) std::cout << "Check Genus:\t      PASSED" << std::endl;
        else                 std::cout << "Check Genus:\t      FAILED (genus: " << genus << ")" << std::endl;

        if(check_conn_comp) std::cout << "Check Conn. Comp.:    PASSED" << std::endl;
        else                std::cout << "Check Conn. Comp.:    FAILED (num cc: " << num_cc << ")" << std::endl;

        if(check_srf_manif) std::cout << "Check Srf. Manifold.: PASSED" << std::endl;
        else                std::cout << "Check Srf. Manifold.: FAILED" << std::endl;

        if(check_bord_edges) std::cout << "Check Srf. Borders:   PASSED" << std::endl;
        else                 std::cout << "Check Srf. Borders:   FAILED (num srf edges: " << num_srf_edges << ")" << std::endl;

        if(check_self_inters) std::cout << "Check Srf. Inters.:   PASSED" << std::endl;
        else                  std::cout << "Check Srf. Inters.:   FAILED (num inters.: " << intersections.size() << ")" << std::endl;
    }
    else
    {
        std::string line = filename;
        std::string sep = "; ";

        line += sep + std::to_string(check_orient) + sep + std::to_string(check_volumes) + sep + std::to_string(check_genus) +
                sep + std::to_string(check_conn_comp) + sep + std::to_string(check_srf_manif) + sep + std::to_string(check_bord_edges) + 
                sep + std::to_string(check_self_inters) + "\n";        

        std::ofstream log(log_filename, std::ios_base::app | std::ios_base::out);
        log << line;
    }

    return EXIT_SUCCESS;
}
