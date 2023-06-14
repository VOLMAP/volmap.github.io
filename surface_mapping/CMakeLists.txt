cmake_minimum_required(VERSION 3.2)

project(surface_mapping)

set(cinolib_DIR "/Users/gianmarco/Code/Cinolib") #path for MacOS

find_package(cinolib REQUIRED)

add_executable(${PROJECT_NAME} 
    main.cpp
    endpoints_and_separatrices.cpp
    mapping.cpp
)

target_link_libraries(${PROJECT_NAME} cinolib)
