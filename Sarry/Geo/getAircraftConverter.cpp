
#include "getAircraftConverter.hpp"
#include "Geo/toEcef.hpp"
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>

namespace nu = boost::numeric::ublas;
using Sarry::AcftCs;
using Sarry::Acs;
using Sarry::Converter;
using Sarry::Ecef;
using Sarry::Nwu;

Sarry::Converter<Sarry::Ecef, Sarry::Nwu> Sarry::getNwuConverter(Geo3 origin)
{
  ///////////////////////////////////////////////////////////////////////
  // Convert to the LTP
  ///////////////////////////////////////////////////////////////////////
  //First, we translate such that org becomes (0,0).
  Ecef ecef = toEcef(origin);
  nu::matrix<double> tx(nu::identity_matrix<double>(4,4));
  tx(0,3) = -ecef.x().value();
  tx(1,3) = -ecef.y().value();
  tx(2,3) = -ecef.z().value();

  nu::matrix<double> r(nu::identity_matrix<double>(4,4));
  double sinLat = sin(origin.lat).value();
  double cosLat = cos(origin.lat).value();
  double sinLon = sin(origin.lon).value();
  double cosLon = cos(origin.lon).value();
  r(0,0) = -sinLat * cosLon;
  r(0,1) = -sinLat * sinLon;
  r(0,2) = cosLat;
  r(1,0) = -sinLon;
  r(1,1) = cosLon;
  r(1,2) = 0;
  r(2,0) = -cosLat * cosLon;
  r(2,1) = -cosLat * sinLon;
  r(2,2) = -sinLat;

  tx = prod(r, tx);

  nu::matrix<double> rz(nu::identity_matrix<double>(4,4));
  rz(1,1) = 1;
  rz(1,2) = 0;
  rz(2,1) = 0;
  rz(2,2) = -1;

  tx = prod(rz, tx);

  return Converter<Ecef, Nwu>(tx);
}

Converter<Nwu, AcftCs> Sarry::getAircraftConverter(Orientation trajectory)
{
  ///////////////////////////////////////////////////////////////////////
  // Rotate into the aircraft tangent plane
  ///////////////////////////////////////////////////////////////////////
  nu::matrix<double> rz(nu::identity_matrix<double>(4,4));
  rz(0,0) =  cos(trajectory.yaw).value();
  rz(0,1) =  sin(trajectory.yaw).value();
  rz(1,0) = -rz(0,1);
  rz(1,1) =  rz(0,0);

  nu::matrix<double> ry(nu::identity_matrix<double>(4,4));
  ry(0,0) = cos(trajectory.pitch).value();
  ry(0,2) = sin(trajectory.pitch).value();
  ry(2,0) = -ry(0,2);
  ry(2,2) =  ry(0,0);

  rz = prod(ry, rz);

  nu::matrix<double> rx(nu::identity_matrix<double>(4,4));
  rx(1,1) =  cos(trajectory.roll).value();
  rx(1,2) = -sin(trajectory.roll).value();
  rx(2,1) = -rx(1,2);
  rx(2,2) =  rx(1,1);

  return Converter<Nwu, AcftCs>(prod(rx, rz));
}

Converter<AcftCs, Acs> Sarry::getAntennaCsConverter(Orientation antenna)
{
  ///////////////////////////////////////////////////////////////////////
  // Finally Rotate into the antenna coordinate system
  ///////////////////////////////////////////////////////////////////////
  nu::matrix<double> rz(nu::identity_matrix<double>(4,4));
  rz(0,0) =  cos(antenna.yaw).value();
  rz(0,1) =  sin(antenna.yaw).value();
  rz(1,0) = -rz(0,1);
  rz(1,1) =  rz(0,0);

  nu::matrix<double> ry(nu::identity_matrix<double>(4,4));
  ry(0,0) = cos(antenna.pitch).value();
  ry(0,2) = sin(antenna.pitch).value();
  ry(2,0) = -ry(0,2);
  ry(2,2) =  ry(0,0);

  rz = prod(ry, rz);

  nu::matrix<double> rx(nu::identity_matrix<double>(4,4));
  rx(1,1) =  cos(antenna.roll).value();
  rx(1,2) =  sin(antenna.roll).value();
  rx(2,1) = -rx(1,2);
  rx(2,2) =  rx(1,1);

  rz = prod(rx, rz);

  return Converter<AcftCs, Acs>(rz);
}

Converter<Ecef, Acs> Sarry::getAntennaCsConverter(
      Geo3 origin, Orientation trajectory, Orientation antenna)
{
  return Converter<Ecef, Acs>(
    Converter<Ecef, AcftCs>(getNwuConverter(origin),
                            getAircraftConverter(trajectory)),
    getAntennaCsConverter(antenna));
}

