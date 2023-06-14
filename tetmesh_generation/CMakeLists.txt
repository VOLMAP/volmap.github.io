cmake_minimum_required(VERSION 3.2)

project(tetmesh_generation)

set(CINOLIB_USES_TETGEN ON)

find_package(cinolib REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} cinolib)
