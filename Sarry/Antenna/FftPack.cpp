
#include "FftPack.hpp"
#include "fftw3.h"

using namespace Sarry;

#pragma comment(lib, "libfftw-3.3.lib")

namespace
{
  fftw_complex* getComplex(const SarData& data)
  {
    assert(data && "Invalid data passed in");
    return static_cast<fftw_complex*>(static_cast<void*>(&(*data.pData)[0]));
  }
}//end namespace

FftPack::FftPack(const SarData& data, int planType)
  : data(data),
    pPlan(fftw_plan_dft_1d(static_cast<int>(data.size),
                           getComplex(data), getComplex(data),
                           planType, FFTW_ESTIMATE), fftw_destroy_plan)
{}

FftPack::~FftPack()
{}


FftPack::operator FftPack::bool_type() const
{ return data && pPlan ? &FftPack::data : NULL; }

