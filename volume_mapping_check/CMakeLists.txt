cmake_minimum_required(VERSION 3.10)
project(volume_mapping_check)

set(CMAKE_CXX_STANDARD 20)

set(CINOLIB_USES_OPENGL_GLFW_IMGUI    ON  ) # used for rendering with OpenGL
set(CINOLIB_USES_TETGEN               OFF ) # used for polygon triangulation
set(CINOLIB_USES_TRIANGLE             OFF ) # used for tetrahedralization
set(CINOLIB_USES_SHEWCHUK_PREDICATES  ON  ) # used for exact geometric tests on input points
set(CINOLIB_USES_INDIRECT_PREDICATES  OFF ) # used for exact geometric tests on implicit points
set(CINOLIB_USES_GRAPH_CUT            OFF ) # used for graph clustering
set(CINOLIB_USES_BOOST                OFF ) # used for 2D polygon operations (e.g. thickening, clipping, 2D booleans...)
set(CINOLIB_USES_VTK                  OFF ) # used just to support VTK file formats
set(CINOLIB_USES_SPECTRA              OFF ) # used for matrix eigendecomposition
set(CINOLIB_USES_CGAL_GMP_MPFR        ON  ) # used for rational numbers with lazy kernel

find_package(cinolib REQUIRED)

add_executable(
        ${PROJECT_NAME} main.cpp
        rationals_io.cpp rationals_io.h
)

target_link_libraries(${CMAKE_PROJECT_NAME} cinolib)