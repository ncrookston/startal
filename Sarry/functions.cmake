
function(enforce_out_of_source_builds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  # check for polluted source tree and disallow in-source builds
  if(EXISTS ${srcdir}/CMakeCache.txt OR EXISTS ${srcdir}/CMakeFiles)
    message("##########################################################")
    message("Found results from an in-source build in source directory.")
    message("Please delete:")
    message("  ${srcdir}/CMakeCache.txt (file)")
    message("  ${srcdir}/CMakeFiles (directory)")
    message("And re-run CMake from an out-of-source directory.")
    message("In-source builds are forbidden!")
    message("##########################################################")
    message(FATAL_ERROR)
  endif()
endfunction()

function(add_sarry_includes_and_defines path_to_sarry)

  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin PARENT_SCOPE )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE )

  include_directories(${path_to_sarry}/../geographiclib-1.6/include)
  include_directories(${path_to_sarry}/../../boost_trunk)
  include_directories(${path_to_sarry})
  include_directories(${path_to_sarry}/../fftw-3.3/api)


  add_definitions(-DFUSION_MAX_VECTOR_SIZE=50 -DPHOENIX_LIMIT=20 -DBOOST_RESULT_OF_NUM_ARGS=20)
  if(MSVC)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS
      -D_SCL_SECURE_NO_WARNINGS -DFFTW_DLL
    )
  endif()

endfunction()

function(add_sarry_link_directories path_to_sarry)

  if(${CMAKE_CL_64})
    set(FFTW_PATH ${path_to_sarry}/../fftw-3.3/fftw-3.3-libs/x64/$(Configuration))
  else()
    set(FFTW_PATH ${path_to_sarry}/../fftw-3.3/fftw-3.3-libs/$(Configuration))
  endif()

  link_directories(${FFTW_PATH})

  if(${CMAKE_CL_64})
    link_directories(${path_to_sarry}/../../boost_trunk/stage/lib/64)
    link_directories(${path_to_sarry}/../geographiclib-1.6/windows/$(Configuration)64)
  else()
    link_directories(${path_to_sarry}/../../boost_trunk/stage/lib/32)
    link_directories(${path_to_sarry}/../geographiclib-1.6/windows/$(Configuration)32)
  endif()

  foreach(link_dir ${EXTRA_LINK_DIRS})
    link_directories(${path_to_sarry}/${link_dir})
  endforeach()
endfunction()

function(add_sarry_static_and_dynamic_libraries target path_to_sarry)
  foreach(dll ${EXTRA_DLLS})
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${path_to_sarry}/${dll} $(OutDir) VERBATIM)
  endforeach()
  target_link_libraries(${target} GeographicLib ${EXTRA_LIBS})
endfunction()

function(add_sarry_test target)
  get_target_property(${target}_exe ${target} LOCATION)
  add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${${target}_exe} --result-code=no --report_level=no VERBATIM
    )

endfunction()
