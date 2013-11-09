#ifndef SARRY_UTILITY_CONSTANTS_HPP
#define SARRY_UTILITY_CONSTANTS_HPP

#include "Utility/Units.hpp"
#include <boost/math/constants/constants.hpp>

namespace Sarry
{
  static const Velocity c = 299792458. * si::meters / si::seconds;
  const Radians pi = boost::math::constants::pi<double>() * si::radians;

}//end Sarry

#endif
