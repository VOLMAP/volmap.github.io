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

#ifndef METRICS_FUNCTIONS_H
#define METRICS_FUNCTIONS_H

#include <cinolib/meshes/tetmesh.h>
#include <cinolib/distortion_energies.h>

struct Data
{
    double upper_bound;

    std::vector<cinolib::vec3d> orig_verts;
    std::vector<cinolib::vec3d> map_verts;

    std::vector<std::vector<uint>> orig_polys;
    std::vector<std::vector<uint>> map_polys;

    std::vector<double> orients;
    std::vector<double> volumes;

    // for each metric: min, max, E
    double conformal[3];
    double dirichlet[3];
    double symmetric_dirichlet[3];
    double arap[3];
    double mips3d[3];
    double tet_radius_ratio[3];

    double total_volume_positive_tets;
    uint inv_tets;
    float perc_inv;
};

void computeOrientAndVolume(Data &s);

void computeInvertedTets(Data &s);

void computeDistortionEnergy(Data &s, cinolib::DistEnergy energy, double lower_d, double upper_d, double *values);

void computeTetRadiusRatio(Data &s);

void printValuesOnScreen(const Data &s);
void printValuesOnFile(std::string &file_in, std::string &file_log, const Data &s);



#endif //METRICS_FUNCTIONS_H
