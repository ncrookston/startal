project(Sarry)
cmake_minimum_required(VERSION 2.8.2)

include(functions.cmake)
enforce_out_of_source_builds()

add_sarry_includes_and_defines(${CMAKE_SOURCE_DIR})

########################### Additional Directories ###########################
add_subdirectory(Utility)
add_subdirectory(Geo)
add_subdirectory(Antenna)
add_subdirectory(Algorithm)
add_subdirectory(NuSar)
add_subdirectory(Srtm)

add_sarry_link_directories(${CMAKE_SOURCE_DIR})

add_executable(Sarry sarry.cpp)
add_executable(PrintTypes printTypes.cpp)
target_link_libraries(PrintTypes Utility)

add_sarry_static_and_dynamic_libraries(Sarry ${CMAKE_SOURCE_DIR})
