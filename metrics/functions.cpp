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

#include "functions.h"
#include <cinolib/predicates.h>
#include <cinolib/parallel_for.h>
#include <cinolib/clamp.h>
#include <math.h>
#include <limits>

using namespace cinolib;

void computeOrientAndVolume(Data &s)
{

    // orient of tet0 (check)
    auto &tet0 = s.orig_polys[0];
    double o = orient3d(s.orig_verts[tet0[0]], s.orig_verts[tet0[1]], s.orig_verts[tet0[2]], s.orig_verts[tet0[3]]);
    assert(o != 0 && "Null volume elements in input mesh");
    assert(o > 0 && "Negative volume elements in input mesh");

    s.orients.resize(s.map_polys.size());
    for(uint ti = 0; ti < s.map_polys.size(); ti++)
    {
        auto &tet = s.map_polys[ti];
        o = orient3d(s.map_verts[tet[0]], s.map_verts[tet[1]], s.map_verts[tet[2]], s.map_verts[tet[3]]);
        s.orients[ti] = o;
    }

    s.volumes.resize(s.map_polys.size());
    s.total_volume_positive_tets = 0;
    for(uint ti = 0; ti < s.map_polys.size(); ti++)
    {
        if(s.orients[ti] <= 0) continue;

        auto &tet = s.map_polys[ti];
        s.volumes[ti] = tet_unsigned_volume(s.map_verts[tet[0]], s.map_verts[tet[1]], s.map_verts[tet[2]], s.map_verts[tet[3]]);
        s.total_volume_positive_tets += s.volumes[ti];
    }

}


void computeInvertedTets(Data &s)
{
    s.inv_tets = 0;

    for(uint ti = 0; ti < s.orients.size(); ti++)
        if(s.orients[ti] <= 0)
            ++s.inv_tets;

    s.perc_inv = (float)s.inv_tets / (float)s.map_polys.size() * 100.0;
}


void computeDistortionEnergy(Data &s, cinolib::DistEnergy energy, double lower_d, double upper_d, double *values)
{
    double E = 0.0; // 1/V_tot' * sum d(ti) * V(ti) foreach ti with V(ti) > 0 (V_tot' is the sum of positive volumes)
    double min = upper_d;
    double max = lower_d;
    double delta = upper_d - lower_d;

    for(uint ti = 0; ti < s.orig_polys.size(); ti++)
    {
        if(s.orients[ti] <= 0) continue;

        auto &o_tet = s.orig_polys[ti];
        auto &m_tet = s.map_polys[ti];
        double d = distortion(s.orig_verts[o_tet[0]], s.orig_verts[o_tet[1]], s.orig_verts[o_tet[2]], s.orig_verts[o_tet[3]],
                              s.map_verts[m_tet[0]], s.map_verts[m_tet[1]], s.map_verts[m_tet[2]], s.map_verts[m_tet[3]],
                              energy);

        if(isinf(d) || isnan(d)) d = upper_d;
        d = clamp(double(d), lower_d, upper_d);
        //d = (d - lower_d) / delta;

        if(d < min) min = d;
        if(d > max) max = d;

        E += d * s.volumes[ti];
    }

    E /= s.total_volume_positive_tets;

    values[0] = min;
    values[1] = max;
    values[2] = E;
}

void computeTetRadiusRatio(Data &s)
{
    double min = std::numeric_limits<double>::infinity();
    double max = -1 * std::numeric_limits<double>::infinity();
    double avg = 0;

    for(uint ti = 0; ti < s.map_polys.size(); ti++)
    {
        auto &tet = s.map_polys[ti];
        double trr = tetrahedron_radius_ratio(s.map_verts[tet[0]], s.map_verts[tet[1]], s.map_verts[tet[2]], s.map_verts[tet[3]]);
        trr = -1 * trr;

        if(trr < min) min = trr;
        if(trr > max) max = trr;
        avg += trr;
    }

    s.tet_radius_ratio[0] = min;
    s.tet_radius_ratio[1] = max;
    s.tet_radius_ratio[2] = avg / s.map_polys.size();
}

void printValuesOnScreen(const Data &s)
{
    std::cout << "Num tets: " << s.map_polys.size() << std::endl;
    std::cout << "Num inv. tets: " << s.inv_tets << " (" << s.perc_inv << "%)" << std::endl;
    std::cout << "Conformal (min/max/E): " << s.conformal[0] << " / " << s.conformal[1] << " / " << s.conformal[2] << std::endl;
    std::cout << "Dirichlet (min/max/E): " << s.dirichlet[0] << " / " << s.dirichlet[1] << " / " << s.dirichlet[2] << std::endl;
    std::cout << "Sym. Dirichlet (min/max/E): " << s.symmetric_dirichlet[0] << " / " << s.symmetric_dirichlet[1] << " / " << s.symmetric_dirichlet[2] << std::endl;
    std::cout << "Arap (min/max/E): " << s.arap[0] << " / " << s.arap[1] << " / " << s.arap[2] << std::endl;
    std::cout << "Mips3d (min/max/E): " << s.mips3d[0] << " / " << s.mips3d[1] << " / " << s.mips3d[2] << std::endl;
    std::cout << "Tet Radius Ratio: (min/max/avg): " << s.tet_radius_ratio[0] << " / " << s.tet_radius_ratio[1] << " /: " << s.tet_radius_ratio[2] << std::endl;
}

void printValuesOnFile(std::string &file_in, std::string &file_log, const Data &s)
{
    std::string line = file_in;
    std::string sep = "; ";

    line += sep + std::to_string(s.map_polys.size()) + sep + std::to_string(s.inv_tets) + sep;
    line += std::to_string(s.conformal[0]) + sep + std::to_string(s.conformal[1]) + sep + std::to_string(s.conformal[2]) + sep;
    line += std::to_string(s.dirichlet[0]) + sep + std::to_string(s.dirichlet[1]) + sep + std::to_string(s.dirichlet[2]) + sep;
    line += std::to_string(s.symmetric_dirichlet[0]) + sep + std::to_string(s.symmetric_dirichlet[1]) + sep + std::to_string(s.symmetric_dirichlet[2]) + sep;
    line += std::to_string(s.arap[0]) + sep + std::to_string(s.arap[1]) + sep + std::to_string(s.arap[2]) + sep;
    line += std::to_string(s.mips3d[0]) + sep + std::to_string(s.mips3d[1]) + sep + std::to_string(s.mips3d[2]) + sep;
    line += std::to_string(s.tet_radius_ratio[0]) + sep + std::to_string(s.tet_radius_ratio[1]) + sep + std::to_string(s.tet_radius_ratio[2]) + "\n";

    std::ofstream log(file_log, std::ios_base::app | std::ios_base::out);
    log << line;
}