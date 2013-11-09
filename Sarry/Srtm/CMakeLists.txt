project(Srtm)
cmake_minimum_required(VERSION 2.8.2)

include(../functions.cmake)
enforce_out_of_source_builds()

add_sarry_includes_and_defines(${CMAKE_SOURCE_DIR})

add_sarry_link_directories(${CMAKE_SOURCE_DIR})

add_library(Srtm SHARED SrtmElevation.cpp Srtm.cpp)

add_sarry_static_and_dynamic_libraries(Srtm ${CMAKE_SOURCE_DIR})

add_subdirectory(Test)
