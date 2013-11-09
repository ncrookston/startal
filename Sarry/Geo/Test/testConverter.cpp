#include "Geo/getAircraftConverter.hpp"
#include "Geo/GeoToIndex.hpp"
#include "Geo/toEcef.hpp"
#include "Geo/toGeo3.hpp"
#include "Geo/toGround.hpp"
#include <boost/test/unit_test.hpp>
#define BOOST_TYPEOF_SILENT
#include <boost/typeof/typeof.hpp>
#include <boost/units/cmath.hpp>

using namespace Sarry;

namespace
{
  void testPoint(const Geo2& origPt, const IPt2& expectedPt,
                 const GeoToIndex& cvtr)
  {
    IPt2 pt = cvtr(origPt);
    BOOST_CHECK_EQUAL(pt.x(), expectedPt.x());
    BOOST_CHECK_EQUAL(pt.y(), expectedPt.y());
    Geo2 gPt = cvtr.reverse(pt);
    BOOST_CHECK_CLOSE(gPt.lat.value(), origPt.lat.value(), 1.e-5);
    BOOST_CHECK_CLOSE(gPt.lon.value(), origPt.lon.value(), 1.e-5);
  }
}//end namespace

BOOST_AUTO_TEST_CASE( nwuOriginTest )
{
  Geo3 origin(1 * degrees, 1 * degrees, 1 * si::meters);
  BOOST_AUTO(toNwu, getNwuConverter(origin));

  Nwu nwu = toNwu(toEcef(origin));
  BOOST_CHECK_CLOSE(nwu.x().value() + 1, 1, 1e-6);
  BOOST_CHECK_CLOSE(nwu.y().value() + 1, 1, 1e-6);
  BOOST_CHECK_CLOSE(nwu.z().value() + 1, 1, 1e-6);
}

BOOST_AUTO_TEST_CASE( nwuDataTest )
{
  BOOST_AUTO(toNwu, getNwuConverter(Geo3(40.99300636695537 * degrees,
    -112.92571470858344 * degrees, 2877.468017578125 * si::meters)));

  Geo3 geoPt(_lat = 41.0703573295 * degrees, _lon = -112.95140134 * degrees,
      _alt = 1313.1215375591848 * si::meters);
  Ecef otherPt = toEcef(geoPt);
  Nwu nwu = toNwu(otherPt);
  BOOST_CHECK_CLOSE(nwu.x().value(), 8592.261689, 1e-6);
  BOOST_CHECK_CLOSE(nwu.y().value(), -2159.293489, 1e-6);
  BOOST_CHECK_CLOSE(nwu.z().value(), -1570.511516, 1e-6);
}

BOOST_AUTO_TEST_CASE( acftDataTest )
{
  Orientation trajectory(_yaw = 25.723621 * degrees,
    _pitch = 3.661663 * degrees, _roll = -5.366232 * degrees);
  BOOST_AUTO(toAcftCs, getAircraftConverter(trajectory));

  Nwu nwu(8592.2621170 * si::meters, -2159.2935966 * si::meters,
    -1570.1941795 * si::meters);
  AcftCs acftcs = toAcftCs(nwu);

  BOOST_CHECK_CLOSE(acftcs.x().value(), 6689.3850640990358, 1e-6);
  BOOST_CHECK_CLOSE(acftcs.y().value(), -5836.9202568730861, 1e-6);
  BOOST_CHECK_CLOSE(acftcs.z().value(), -1462.0247138814439, 1e-6);
}

BOOST_AUTO_TEST_CASE( acsDataTest )
{
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);
  BOOST_AUTO(toAcs, getAntennaCsConverter(antenna));

  AcftCs acftcs(6689.3850640990358 * si::meters,
    -5836.9202568730861 * si::meters, -1462.0247138814439 * si::meters);
  Acs acs = toAcs(acftcs);

  BOOST_CHECK_CLOSE(acs.x().value(), 5161.1334843279747, 1e-6);
  BOOST_CHECK_CLOSE(acs.y().value(), 6689.3850640990358, 1e-6);
  BOOST_CHECK_CLOSE(acs.z().value(), 3093.5183054321933, 1e-6);
}

BOOST_AUTO_TEST_CASE( fullDataTest )
{
  Geo3 origin(40.99300636695537 * degrees, -112.92571470858344 * degrees,
    2877.468017578125 * si::meters);
  Orientation trajectory(_yaw = 25.723621 * degrees,
    _pitch = 3.661663 * degrees, _roll = -5.366232 * degrees);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);
  BOOST_AUTO(toAcs, getAntennaCsConverter(origin, trajectory, antenna));

  Geo3 geoPt(_lat = 41.0703573295 * degrees, _lon = -112.95140134 * degrees,
      _alt = 1313.1215375591848 * si::meters);
  Ecef otherPt = toEcef(geoPt);

  Acs acs = toAcs(otherPt);

  BOOST_CHECK_CLOSE(acs.x().value(), 5161.37718, 1e-6);
  BOOST_CHECK_CLOSE(acs.y().value(), 6689.364459, 1e-6);
  BOOST_CHECK_CLOSE(acs.z().value(), 3093.316093, 1e-6);
}

BOOST_AUTO_TEST_CASE( fullDataInversion )
{
  Geo3 origin(40.99300636695537 * degrees, -112.92571470858344 * degrees,
    2877.468017578125 * si::meters);
  Orientation trajectory(_yaw = 25.723621 * degrees,
    _pitch = 3.661663 * degrees, _roll = -5.366232 * degrees);
  Orientation antenna(_yaw = -90 * degrees, _pitch = -45 * degrees);
  BOOST_AUTO(toAcs, getAntennaCsConverter(origin, trajectory, antenna));

  Geo3 geoPt(_lat = 41.0703573295 * degrees, _lon = -112.95140134 * degrees,
      _alt = 1313.1215375591848 * si::meters);
  Ecef otherPt = toEcef(geoPt);

  Acs acs = toAcs(otherPt);

  BOOST_AUTO(toEcef, toAcs.invert());

  Ecef ecef = toEcef(acs);

  BOOST_CHECK_CLOSE(otherPt.x().value(), ecef.x().value(), 1e-6);
  BOOST_CHECK_CLOSE(otherPt.y().value(), ecef.y().value(), 1e-6);
  BOOST_CHECK_CLOSE(otherPt.z().value(), ecef.z().value(), 1e-6);
}

BOOST_AUTO_TEST_CASE( GeoToIndexTest )
{
  Geo2 corners[4] = {
    Geo2(_lat = 41.0703573295 * degrees, _lon = -112.95140134 * degrees),
    Geo2(_lat = 41.0703571713 * degrees, _lon = -112.950939488 * degrees),
    Geo2(_lat = 41.0699953623 * degrees, _lon = -112.950939501 * degrees),
    Geo2(_lat = 41.0699952356 * degrees, _lon = -112.951400491 * degrees)
  };

  Meters resolution = 1. * si::meters;

  GeoToIndex cvtr(corners[0], corners[1], corners[3], resolution);

  testPoint(corners[0], IPt2(0 , 0 ), cvtr);
  testPoint(corners[1], IPt2(39, 0 ), cvtr);
  testPoint(corners[2], IPt2(39, 40), cvtr);
  testPoint(corners[3], IPt2(0 , 40), cvtr);

  testPoint(Geo2(_lat = 41.0694114871236 * degrees,
    _lon = -112.950922302496 * degrees), IPt2(40,105), cvtr);

  testPoint(Geo2(_lat = 41.069400138501 * degrees,
    _lon = -112.951477530541 * degrees), IPt2(-7,106), cvtr);
}

