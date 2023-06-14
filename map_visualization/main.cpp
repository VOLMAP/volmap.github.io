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

#include <cinolib/gl/glcanvas.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/volume_mesh_controls.h>
#include <cinolib/distortion_energies.h>
#include <cinolib/clamp.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

DrawableTetmesh<> m1, m2;
float min_dist = 0;
float max_dist = 20;

DistEnergy energy = DistEnergy::MIPS3D;
int energy_id = 4;
// 0 -> CONFORMAL
// 1 -> DIRICHLET
// 2 -> SYMMETRIC_DIRICHLET
// 3 -> ARAP
// 4 -> MIPS3D


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void map_recolor()
{
    float delta = max_dist - min_dist;
    std::vector<float> dist(m1.num_polys());
    PARALLEL_FOR(0,m1.num_polys(),1000000,[&](uint pid)
    {
        double d = distortion(m1.poly_vert(pid,0), m1.poly_vert(pid,1), m1.poly_vert(pid,2), m1.poly_vert(pid,3),
                              m2.poly_vert(pid,0), m2.poly_vert(pid,1), m2.poly_vert(pid,2), m2.poly_vert(pid,3),
                              energy);

        if(isinf(d) || isnan(d)) d = max_dist;
        dist[pid] = clamp(float(d), min_dist, max_dist);

        dist[pid] = (dist[pid] - min_dist)/delta;

        m1.poly_data(pid).color = Color::red_ramp_01(dist[pid]);
        m2.poly_data(pid).color = Color::red_ramp_01(dist[pid]);
    });
    m1.updateGL();
    m2.updateGL();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    std::string filename;
    std::string map_name;

    if(argc == 3)
    {
        filename = argv[1];
        map_name = argv[2];
    }
    else
    {
        std::cout << "Command Syntax Error! Correct syntax: ./map_visualization <model.mesh> <vol_mapping.mesh>" << std::endl;
        return EXIT_FAILURE;
    }

    m1 = DrawableTetmesh<>(filename.c_str());
    m2 = DrawableTetmesh<>(map_name.c_str());

    GLcanvas gui1;
    GLcanvas gui2;
    gui1.push(&m1);    
    gui2.push(&m2);
    gui1.push(new VolumeMeshControls<DrawableTetmesh<>>(&m1,&gui1));
    gui1.push(new VolumeMeshControls<DrawableTetmesh<>>(&m2,&gui1));
    gui1.show_side_bar = true;
    gui1.callback_app_controls = []()
    {
        bool update = false;
        if(ImGui::Combo("##Energy",&energy_id,"CONFORMAL\0DIRICHLET\0SYMMETRIC_DIRICHLET\0ARAP\0MIPS3D\0"))
        {
            switch(energy_id)
            {
                case 0 : energy   = DistEnergy::CONFORMAL;
                         min_dist = 1.f;
                         max_dist = std::max(max_dist,2.f);
                         break;

                case 1 : energy   = DistEnergy::DIRICHLET;
                         min_dist = 3.f;
                         max_dist = std::max(max_dist,4.f);
                         break;

                case 2 : energy   = DistEnergy::SYMMETRIC_DIRICHLET;
                         min_dist = 6.f;
                         max_dist = std::max(max_dist,7.f);
                         break;

                case 3 : energy   = DistEnergy::ARAP;
                         min_dist = 0.f;
                         max_dist = std::max(max_dist,1.f);
                         break;

                case 4 : energy   = DistEnergy::MIPS3D; break;
                         min_dist = 1;
                         max_dist = std::max(max_dist,2.f);
                         break;
            }
            update = true;
        }

        if(ImGui::InputScalar("Min", ImGuiDataType_Float, &min_dist)) update = true;
        if(ImGui::InputScalar("Max", ImGuiDataType_Float, &max_dist)) update = true;
        if(update) map_recolor();
    };

    map_recolor();
    return gui1.launch({&gui2});
}
