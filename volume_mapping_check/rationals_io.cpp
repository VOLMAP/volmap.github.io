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