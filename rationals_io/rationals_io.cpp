/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2024 Federico Meloni, Gianmarco Cherchi, Marco Livesu                   *
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
 *      Federico Meloni (federico.meloni3@unica.it)                                      *
 *      https://www.federicheddu.github.io                                               *
 *                                                                                       *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://www.gianmarcocherchi.com                                                 *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 * ***************************************************************************************/

#include "rationals_io.h"

void save_RT(const std::vector<CGAL_Q> &rationals, const std::string &path, int base) {

    //file opening
    FILE *f = nullptr;
    f = fopen(path.c_str(), "w");
    if(f == nullptr) exit(1);

    //write all the rationals numbers
    for(auto &rt : rationals) {
        mpq_out_str(f, base, rt.exact().mpq());
        fprintf(f, "\n");
    }

    //close the file
    fclose(f);
}

void load_RT(std::vector<CGAL_Q> &rationals, const std::string &path) {

    //clear
    rationals.clear();

    //file txt with the rationals
    FILE *file = nullptr;
    file = fopen(path.c_str(), "r");
    if(file == nullptr) exit(1);

    //aux variable to store the last read rational
    mpq_ptr aux = nullptr;
    aux = (mpq_ptr) malloc(sizeof(__mpq_struct));
    if(aux == nullptr) exit(1);

    //read until the end of the file
    int check;
    do {
        check = mpq_inp_str(aux, file, 10);
        if(check != 0) rationals.emplace_back(aux);
    } while(check != 0);

    //free and close
    free(aux);
    fclose(file);

}