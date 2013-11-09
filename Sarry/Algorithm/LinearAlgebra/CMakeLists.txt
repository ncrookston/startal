
project(LinearAlgebraAlgorithms)
cmake_minimum_required(VERSION 2.8.2)

include(../../functions.cmake)
enforce_out_of_source_builds()

include_directories(${CMAKE_SOURCE_DIR}/../jpeg-8c)
add_sarry_includes_and_defines(${CMAKE_SOURCE_DIR})

add_sarry_link_directories(${CMAKE_SOURCE_DIR})

add_library(LinearAlgebraAlgorithms SHARED Doi.cpp FHd.cpp LinearAlgebra.cpp MatchedMatrix.cpp MaximumLikelihood.cpp RegionSummary.cpp RegisterAlgorithms.cpp Roi.cpp)

add_sarry_static_and_dynamic_libraries(LinearAlgebraAlgorithms ${CMAKE_SOURCE_DIR})
target_link_libraries(LinearAlgebraAlgorithms GeographicLib Algorithm Antenna Geo Utility)

