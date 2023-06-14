cmake_minimum_required(VERSION 3.20)
project(surface_mapping_check)

set(CINOLIB_USES_SHEWCHUK_PREDICATES ON)

find_package(cinolib REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} cinolib)
