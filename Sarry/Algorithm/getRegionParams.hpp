#ifndef SARRY_ALGORITHM_GETREGIONPARAMS_HPP
#define SARRY_ALGORITHM_GETREGIONPARAMS_HPP

#include "Geo/Geo.hpp"

namespace Sarry
{
  void getRegionParams(int argc, char* argv[],
                       Geo2 corners[4], Meters& rResolution);

}//end Sarry

#endif
