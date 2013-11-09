#include "Geo/getAircraftConverter.hpp"
#include "Geo/GeoToIndex.hpp"
#include "Geo/toEcef.hpp"
#include "Geo/toGeo3.hpp"
#include "Geo/toGround.hpp"
#include "Srtm/SrtmElevation.hpp"
#include <boost/test/unit_test.hpp>
#define BOOST_TYPEOF_SILENT
#include <boost/typeof/typeof.hpp>
#include <boost/units/cmath.hpp>

using namespace Sarry;

namespace
{
  SrtmElevation getElevation()
  {
    int argc = 1;
    char* argv[] = {"Unused"};
    return SrtmElevation(argc, argv);
  }
}

BOOST_AUTO_TEST_CASE( pointProjection )
{
  Geo3 origin(40.99300636695537 * degrees, -112.92571470858344 * degrees,
    2877.468017578125 * si::meters);
  Orientation trajectory(_yaw = 25.723621 * degrees,
    _pitch = 3.661663 * degrees, _roll = -5.366232 * degrees);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);
  BOOST_AUTO(toAcs, getAntennaCsConverter(origin, trajectory, antenna));

  Geo2 pt(41.0703573295 * degrees, -112.95140134 * degrees);
  SrtmElevation elevation = getElevation();
  Geo3 geoPt(pt, elevation(pt));
  Acs acs = toAcs(toEcef(geoPt));

  ToGround toGround(origin, getAntennaCsConverter(origin, trajectory, antenna),
    elevation, ToGround::RETURN_CLOSEST);

  Pointing pointing(atan2(acs.y(), acs.x()), atan2(acs.z(), acs.x()));
  Geo3 estPt = toGround(pointing);

  BOOST_CHECK_CLOSE(estPt.lat.value(), geoPt.lat.value(), 1e-5);
  BOOST_CHECK_CLOSE(estPt.lon.value(), geoPt.lon.value(), 1e-5);
  BOOST_CHECK(
    abs(estPt.alt - geoPt.alt).value() < ToGround::ALT_TOLERANCE.value());
}

BOOST_AUTO_TEST_CASE( elvDetermination )
{
  Geo3 origin(40.99300636695537 * degrees, -112.92571470858344 * degrees,
    2877.468017578125 * si::meters);
  Orientation trajectory(_yaw = 25.723621 * degrees);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);
  BOOST_AUTO(toAcs, getAntennaCsConverter(origin, trajectory, antenna));
  BOOST_AUTO(fromAcs, toAcs.invert());

  Geo3 geoPt(toGeo3(fromAcs(
    Acs(500. * si::meters, 0. * si::meters, 0. * si::meters))));
  SrtmElevation elevation = getElevation();
  geoPt.alt = elevation(geoPt.geo2());
  Acs acs = toAcs(toEcef(geoPt));

  ToGround toGround(origin, getAntennaCsConverter(origin, trajectory, antenna),
    elevation, ToGround::RETURN_CLOSEST);

  Geo3 estPt = toGround(distance(acs));

  BOOST_CHECK_CLOSE(estPt.lat.value(), geoPt.lat.value(), 1e-6);
  BOOST_CHECK_CLOSE(estPt.lon.value(), geoPt.lon.value(), 1e-6);
  BOOST_CHECK(abs(estPt.alt - geoPt.alt).value()
    < ToGround::ALT_TOLERANCE.value());

  BOOST_CHECK_CLOSE(distance(acs).value(),
    distance(toAcs(toEcef(estPt))).value(), 1e-6);
}

BOOST_AUTO_TEST_CASE( elvDataDetermination2 )
{
  Geo3 origin(41.061621803364375 * degrees, -112.90834898283035 * degrees,
    2905.3403320312500 * si::meters);
  Orientation trajectory(
    _yaw = 6.2305119859397990 * si::radians,
    _pitch = 0.079522122549355229 * si::radians,
    _roll = -0.0074395532030904904 * si::radians);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);

  SrtmElevation elevation = getElevation();
  ToGround toGround(origin, getAntennaCsConverter(origin, trajectory, antenna),
    elevation, ToGround::RETURN_CLOSEST);

  Geo3 nearPt = toGround(4025.4886360680002 * si::meters);
  Geo3 farPt = toGround(4074.0550142640000 * si::meters);
}

BOOST_AUTO_TEST_CASE( elvDataDetermination )
{
  Geo3 origin(41.062899385199124 * degrees, -112.90824630353656 * degrees,
    2903.8005371093750 * si::meters);
  Orientation trajectory(
    _yaw = 6.2347689137985904 * si::radians,
    _pitch = 0.077916978142885118 * si::radians,
    _roll = 0.010574653231860697 * si::radians);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);

  SrtmElevation elevation = getElevation();
  ToGround toGround(origin, getAntennaCsConverter(origin, trajectory, antenna),
    elevation, ToGround::RETURN_CLOSEST);

  Geo3 nearPt = toGround(4002.1048243440005 * si::meters);
  Geo3 farPt = toGround(4050.0716176239998 * si::meters);
}

BOOST_AUTO_TEST_CASE( elvDataDetermination3 )
{
  Geo3 origin(41.064862976901914 * degrees, -112.90806990678600 * degrees,
    2902.1072942763431 * si::meters);
  Orientation trajectory(
    _yaw = 6.2415716224248108 * si::radians,
    _pitch = 0.088855524624560339 * si::radians,
    _roll = 0.0065093462600912411 * si::radians);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);

  SrtmElevation elevation = getElevation();
  ToGround toGround(origin, getAntennaCsConverter(origin, trajectory, antenna),
    elevation, ToGround::RETURN_CLOSEST);

  Geo3 nearPt = toGround(3977.5218427880004 * si::meters);
  Geo3 farPt = toGround(4023.6898813200000 * si::meters);
}
