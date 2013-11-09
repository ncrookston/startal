
set(EXTRA_DLLS ${EXTRA_DLLS} ${FFTW_PATH}/libfftw-3.3.dll)

set(EXTRA_LIBS ${EXTRA_LIBS} Antenna PARENT_SCOPE)

add_library(Antenna PerPulseInfo.cpp getPulsesOfInterest.cpp FftPack.cpp)
