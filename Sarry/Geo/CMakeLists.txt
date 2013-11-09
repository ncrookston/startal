add_library(Geo toGround.cpp toEcef.cpp getAircraftConverter.cpp toGeo3.cpp GeoToIndex.cpp)

set(EXTRA_LIBS ${EXTRA_LIBS} Geo PARENT_SCOPE)

add_subdirectory(Test)
