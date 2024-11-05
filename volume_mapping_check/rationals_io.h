#ifndef VOLUME_MAPPING_CHECK_RATIONALS_IO_H
#define VOLUME_MAPPING_CHECK_RATIONALS_IO_H

#include <cinolib/rationals.h>

using namespace cinolib;

//output functions
void save_RT(const std::vector<CGAL_Q> &rationals, const std::string &path, int base = 10);
//input functions
void load_RT(std::vector<CGAL_Q> &rationals, const std::string &path);

#endif //VOLUME_MAPPING_CHECK_RATIONALS_IO_H
