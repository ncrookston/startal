#ifndef SARRY_ANTENNA_GETPULSESOFINTEREST_HPP
#define SARRY_ANTENNA_GETPULSESOFINTEREST_HPP

#include "PerPulseInfo.hpp"
#include "Geo/Geo.hpp"
#include <vector>

namespace Sarry
{
  class Chirp;
  class PerPulseInfoList;
  class PerPulseInfoLoader;

  std::vector<PerPulseInfo> getPulsesOfInterest(
      const PerPulseInfoLoader& infoLoader, const Chirp& chirp,
      const std::vector<Geo3>& geoPts);

}//end Sarry

#endif
