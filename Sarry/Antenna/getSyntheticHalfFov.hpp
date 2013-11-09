#ifndef SARRY_ANTENNA_GETSYNTHETICHALFFOV_HPP
#define SARRY_ANTENNA_GETSYNTHETICHALFFOV_HPP

#include "Chirp.hpp"
#include "Utility/Constants.hpp"
#include <boost/units/cmath.hpp>

namespace Sarry
{
  Radians getSyntheticHalfFov(const Chirp& chirp, Meters avgDist)
  {
    Meters wavelength
      = c / (chirp.getStartTxFrequency() + chirp.getBandWidth() / 2.);
    Meters L = chirp.getAntennaLength();
    //Using Cummings's and Wong's approximation:
    Meters Lsar = .886 * avgDist * wavelength / L;
    return .886 * wavelength / Lsar * si::radians;

    //BUG: if wavelength > L this is not in asin's domain.
    //Radians halfFov = asin(wavelength / L);
    //Meters Lsar = 2. * avgDist * wavelength / (L * cos(halfFov));
    //return asin(wavelength / Lsar);
  }
}//end Sarry

#endif
