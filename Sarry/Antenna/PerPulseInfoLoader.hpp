#ifndef SARRY_ANTENNA_PERPULSEINFOLOADER_HPP
#define SARRY_ANTENNA_PERPULSEINFOLOADER_HPP

#include "Antenna/FftPack.hpp"
#include <cstddef>

namespace Sarry
{
  class PerPulseInfo;

  class PerPulseInfoLoader
  {
  public:

    virtual ~PerPulseInfoLoader() {}

    virtual PerPulseInfo operator[](std::size_t idx) const = 0;

    virtual bool isValid(std::size_t idx) const = 0;

    /** This method may only safely be called from a single thread. */
    virtual std::pair<FftPack, FftPack> getMatchedFilterPacks() const = 0;

  };//end PerPulseInfoLoader

}//end Sarry

#endif

