cmake_minimum_required(VERSION 3.2)

project(metrics)

set(CINOLIB_USES_SHEWCHUK_PREDICATES ON)

find_package(cinolib REQUIRED)

add_executable(${PROJECT_NAME} 
    main.cpp
    functions.cpp
)

target_link_libraries(${PROJECT_NAME} cinolib)
