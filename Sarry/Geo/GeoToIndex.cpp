
#include "GeoToIndex.hpp"

#include "Utility/round.hpp"
#include "Geo/Euclid.hpp"
#include "GeographicLib/Geodesic.hpp"

using namespace Sarry;

GeoToIndex::GeoToIndex(const Geo2& ul, const Geo2& ur, const Geo2& ll,
           Meters resolution)
  : m_ul(ul), m_ba(ur - ul), m_ca(ll - ul), m_xScale(), m_yScale()
{
  GeographicLib::Geodesic geodesic(GeographicLib::Constants::WGS84_a(),
    GeographicLib::Constants::WGS84_r());
  Meters x;
  geodesic.Inverse(ul.lat.value(), ul.lon.value(),
                   ur.lat.value(), ur.lon.value(),
                   boost::units::quantity_cast<double&>(x));

  m_xScale = x / resolution;

  Meters y;
  geodesic.Inverse(ul.lat.value(), ul.lon.value(),
                   ll.lat.value(), ll.lon.value(),
                   boost::units::quantity_cast<double&>(y));
  m_yScale = y / resolution;
}

IPt2 GeoToIndex::operator()(const Geo2& absPt) const
{
  Geo2 pt = absPt - m_ul;
  double s = (m_ca.lat * pt.lon - m_ca.lon * pt.lat)
    / (m_ba.lon * m_ca.lat - m_ca.lon * m_ba.lat);
  
  double t = (m_ba.lat * pt.lon - m_ba.lon * pt.lat)
    / (m_ca.lon * m_ba.lat - m_ba.lon * m_ca.lat);

  return IPt2(round<int>(s * m_xScale), round<int>(t * m_yScale));
}

Geo2 GeoToIndex::reverse(const IPt2& pt) const
{
  //return m_ba * x(pt) / m_xScale + m_ca * y(pt) / m_yScale + m_ul;
  double s = pt.x() / m_xScale;
  double t = pt.y() / m_yScale;
  Degrees lat = m_ba.lat * s + m_ca.lat * t;
  Degrees lon = m_ba.lon * s + m_ca.lon * t;
  return Geo2(_lat = lat + m_ul.lat, _lon = lon + m_ul.lon);
}
