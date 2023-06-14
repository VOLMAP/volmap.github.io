cmake_minimum_required(VERSION 3.2)
project(mesh_converter)

set(CINOLIB_USES_SHEWCHUK_PREDICATES ON)
set(CINOLIB_USES_VTK ON)

find_package(cinolib REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} cinolib)
