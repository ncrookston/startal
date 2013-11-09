
#include "toGeo3.hpp"

#include "GeographicLib/Geocentric.hpp"

Sarry::Geo3 Sarry::toGeo3(const Ecef& pt)
{
  GeographicLib::Geocentric geocentric(GeographicLib::Constants::WGS84_a(),
    GeographicLib::Constants::WGS84_r());

  Geo3 geo3;
  geocentric.Reverse(pt.x().value(), pt.y().value(), pt.z().value(),
    boost::units::quantity_cast<double&>(geo3.lat),
    boost::units::quantity_cast<double&>(geo3.lon),
    boost::units::quantity_cast<double&>(geo3.alt));

  return geo3;
}

