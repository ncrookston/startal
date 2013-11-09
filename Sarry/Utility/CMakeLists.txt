
include_directories(../../zlib-1.2.5)
add_library(Utility writeMagImage.cpp loadSharedLibraries.cpp printToStdout.cpp ZipFile.cpp)

set(PATH_TO_ZLIB_DLL ../zlib-1.2.5/contrib/vstudio/vc10/x64/ZlibDll$(Configuration))

if(${CMAKE_CL_64})
  set(JPEG_LINK_DIR ../jpeg-8c/x64/$(Configuration))
else()
  set(JPEG_LINK_DIR ../jpeg-8c/win32/$(Configuration))
endif()

set(EXTRA_LINK_DIRS ${EXTRA_LINK_DIRS} ${PATH_TO_ZLIB_DLL} ${JPEG_LINK_DIR} PARENT_SCOPE)

set(EXTRA_LIBS ${EXTRA_LIBS} Utility zlibwapi libjpeg PARENT_SCOPE)
set(EXTRA_DLLS ${EXTRA_DLLS} ${PATH_TO_ZLIB_DLL}/zlibwapi.dll PARENT_SCOPE)

add_subdirectory(Test)

