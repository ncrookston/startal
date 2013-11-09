project(Algorithm)
cmake_minimum_required(VERSION 2.8.2)

include(../functions.cmake)

include_directories(${CMAKE_SOURCE_DIR}/../jpeg-8c)

add_library(Algorithm DataSummary.cpp getRegionParams.cpp writeKmz.cpp)

set(EXTRA_LIBS ${EXTRA_LIBS} Algorithm PARENT_SCOPE)

add_subdirectory(LinearAlgebra)

