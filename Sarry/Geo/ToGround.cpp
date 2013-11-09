
#include "ToGround.hpp"
#include "Geo/Elevation.hpp"
#include "Geo/getAircraftConverter.hpp"
#include "Geo/toEcef.hpp"
#include "Geo/toGeo3.hpp"

#include "GeographicLib/Geocentric.hpp"
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/units/cmath.hpp>

using namespace Sarry;
namespace units = boost::units;
using boost::tie;

const std::size_t NUM_ITERATIONS = 500;
const Meters ToGround::ALT_TOLERANCE = .01 * si::meters;
const Radians pi = boost::math::constants::pi<double>() * si::radians;

ToGround::ToGround(Geo3 origin, const Converter<Ecef, Acs>& toAcs,
    const Elevation& elevation, OnBadResolution onBadResolution)
  : m_toAcs(toAcs),
    m_toEcef(m_toAcs.invert()),
    m_geoDown(),
    m_agl(0),
    m_elevation(elevation),
    m_onBadResolution(onBadResolution)
{
  Acs gDown = m_toAcs(toEcef(Geo3(origin.geo2(), elevation(origin.geo2()))));
  //Distance from the origin to the ground point directly below.
  m_agl = distance(gDown);

  m_geoDown.azimuth = atan2(gDown.y(), gDown.x());
  m_geoDown.elevation = atan2(gDown.z(), gDown.x());
}

Geo3 ToGround::operator()(const Pointing& pointing) const
{
  const Radians thetaGround = getAngleToNadir(pointing);

  Meters lastDistance = m_agl;
  Meters lastDiff = m_agl;

  for(std::size_t i = 0; i < NUM_ITERATIONS; ++i)
  {
    //Overshooting by about 1.5 requires about 15 iterations -- no overshoot
    // requires about 30 for the limited tests I performed.
    //We assume the ground is flat for our estimation.
    Meters range = 1.5 * lastDistance / cos(thetaGround);

    Meters heightDiff;
    Geo3 pt;
    tie(heightDiff, pt) = estimateDifference(pointing, range);

    if(abs(heightDiff) < ALT_TOLERANCE)
      return pt;
    if(abs(heightDiff) > lastDiff)
      throw std::runtime_error("Projection not apparantly on to earth");
    if(i == NUM_ITERATIONS - 1 && m_onBadResolution == RETURN_CLOSEST)
      return pt;

    lastDiff = abs(heightDiff);

    lastDistance += heightDiff;
  }

  throw std::runtime_error("Never resolved elevation to desired accuracy");
}

Geo3 ToGround::operator()(Meters range) const
{
  //Initial try assumes given range is close to the actual range.
  Pointing pointing(_azimuth = 0 * si::radians, _elevation = 0 * si::radians);
  Meters heightDiff;
  Geo3 pt;
  tie(heightDiff, pt) = estimateDifference(pointing, range);
  pt.alt -= heightDiff;
  //This will move pt out of the acs xy plane (this should always be in the
  // xy plane). We could move it back into that plane, or we could ensure that
  // we don't return this point.  Hence the i>0 condition below.

  for(std::size_t i = 0; i < NUM_ITERATIONS; ++i)
  {
    Acs acsPt = m_toAcs(toEcef(pt));
    acsPt.y() = 0. * si::meters;
    Meters rangeDiff = range - distance(acsPt);

    heightDiff = rangeDiff;
    //Don't return pt on the first time through -- it may not reside in the
    // correct location.
    if(i > 0 && abs(rangeDiff) < .001 * ALT_TOLERANCE)
      return pt;
    //Else if we're within a meter and we're almost out of iterations
    // (e.g., perhaps we've been ping-ponging), return pt anyway.
    if(abs(rangeDiff) < 1. * si::meters && i > NUM_ITERATIONS - 50)
      return pt;
    if(i == NUM_ITERATIONS - 1 && m_onBadResolution == RETURN_CLOSEST)
      return pt;

    acsPt.x() += .3 * rangeDiff;
    pt = toGeo3(m_toEcef(acsPt));
    pt.alt = m_elevation(pt.geo2());
  }

  throw std::runtime_error("Never resolved elevation to desired accuracy");
}

boost::tuple<Meters, Geo3> ToGround::estimateDifference(
  const Pointing& pointing, Meters range) const
{
  Radians thetaAcs = atan(tan(pointing.elevation) * cos(pointing.azimuth));

  Acs pt;
  Meters xyPlane = range * cos(thetaAcs);
  pt.x() = xyPlane * cos(pointing.azimuth);
  pt.y() = xyPlane * sin(pointing.azimuth);
  pt.z() = range * sin(thetaAcs);

  Ecef ecef = m_toEcef(pt);

  GeographicLib::Geocentric geocentric(GeographicLib::Constants::WGS84_a(),
    GeographicLib::Constants::WGS84_r());

  Geo3 geo = toGeo3(ecef);

  return boost::make_tuple(geo.alt - m_elevation(geo.geo2()), geo);
}

Radians ToGround::getAngleToNadir(const Pointing& pointing) const
{
  Radians azGround = m_geoDown.azimuth - pointing.azimuth;
  Radians elGround = m_geoDown.elevation - pointing.elevation;
  return atan(tan(elGround) * cos(azGround));
}
