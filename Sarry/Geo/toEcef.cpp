
#include "GeographicLib/Geocentric.hpp"

#include "toEcef.hpp"

Sarry::Ecef Sarry::toEcef(const Geo3& pt)
{
  GeographicLib::Geocentric geocentric(GeographicLib::Constants::WGS84_a(),
    GeographicLib::Constants::WGS84_r());

  Ecef ecef;
  geocentric.Forward(pt.lat.value(), pt.lon.value(), pt.alt.value(),
    boost::units::quantity_cast<double&>(ecef.x()),
    boost::units::quantity_cast<double&>(ecef.y()),
    boost::units::quantity_cast<double&>(ecef.z()));

  return ecef;
}
