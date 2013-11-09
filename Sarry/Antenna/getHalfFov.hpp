#ifndef SARRY_ANTENNA_GETHALFFOV_HPP
#define SARRY_ANTENNA_GETHALFFOV_HPP

#include "Chirp.hpp"
#include "Utility/Constants.hpp"
#include <boost/units/cmath.hpp>

namespace Sarry
{
  inline Radians getHalfFov(const Chirp& chirp)
  {
    boost::optional<Radians> optRads = chirp.getHalfBeamwidth();
    if(!optRads)
    {
      Meters wavelength
        = c / (chirp.getStartTxFrequency() + chirp.getBandWidth() / 2.);
      Meters L = chirp.getAntennaLength();
      //Using Cummings's and Wong's approximation, which is inappropriate for
      // wide-beam antennas.
      return .886 * wavelength / L * si::radians;
    }
    //else
    return *optRads;
  }
}//end Sarry

#endif
