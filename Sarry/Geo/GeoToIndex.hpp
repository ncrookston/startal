#ifndef SARRY_GEO_GEOTOINDEX_HPP
#define SARRY_GEO_GEOTOINDEX_HPP

#include "Geo/Euclid.hpp"
#include "Geo/Geo.hpp"

namespace Sarry
{
  /** This converts from a geo coordinate to a relative xy index.  It's based
   *  on a system where a is the origin, b is 1 on the x-axis, c is 1 on the
   *  y-axis.  p, an arbitrary point, is described as p = (b-a)*s + (c-a)*t.
   */
  class GeoToIndex
  { 
  public:
    GeoToIndex(const Geo2& ul, const Geo2& ur, const Geo2& ll,
               Meters resolution);

    IPt2 operator()(const Geo2& absPt) const;

    Geo2 reverse(const IPt2& pt) const;

  private:
    Geo2 m_ul;
    Geo2 m_ba, m_ca;
    double m_xScale, m_yScale;
  };//end GeoToIndex
}//end Sarry

#endif

