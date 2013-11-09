#ifndef SARRY_GEO_TOGROUND_HPP
#define SARRY_GEO_TOGROUND_HPP

#include "Geo/Converter.hpp"
#include "Geo/Euclid.hpp"
#include "Geo/Geo.hpp"
#include "Geo/Orientation.hpp"
#include "Geo/Pointing.hpp"
#include <boost/tuple/tuple.hpp>

namespace Sarry
{
  class Elevation;

  /** Given an antenna coordinate system and an azimuth/elevation pair, finds
   *  the ground point at the intersection, or throws an exception.
   */
  class ToGround
  {
  public:
    static const Meters ALT_TOLERANCE;
    enum OnBadResolution { THROW_IF_BAD, RETURN_CLOSEST };
    ToGround(Geo3 origin, const Converter<Ecef, Acs>& toAcs,
             const Elevation& elevation, OnBadResolution onBadResolution);

    Geo3 operator()(const Pointing& pointing) const;

    Geo3 operator()(Meters range) const;

  private:
    boost::tuple<Meters, Geo3> estimateDifference(
      const Pointing& pointing, Meters z) const;

    Radians getAngleToNadir(const Pointing& pointing) const;

    Converter<Ecef, Acs> m_toAcs;
    Converter<Acs, Ecef> m_toEcef;
    Pointing m_geoDown;
    Meters m_agl;
    const Elevation& m_elevation;
    OnBadResolution m_onBadResolution;
  };//end ToGround
}//end Sarry

#endif

