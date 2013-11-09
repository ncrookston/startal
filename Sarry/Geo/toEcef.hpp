#ifndef SARRY_GEO_TOECEF_HPP
#define SARRY_GEO_TOECEF_HPP

#include "Geo/Euclid.hpp"
#include "Geo/Geo.hpp"

namespace Sarry
{
  Ecef toEcef(const Geo3& pt);
}//end Sarry

#endif

