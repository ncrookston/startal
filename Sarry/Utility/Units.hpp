#ifndef SARRY_UNITS_HPP
#define SARRY_UNITS_HPP

#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/frequency.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/si/time.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace Sarry
{
  namespace si = boost::units::si;
  using boost::units::degree::degrees;

  typedef boost::units::quantity<si::length> Meters;
  typedef boost::units::quantity<si::plane_angle> Radians;
  typedef boost::units::quantity<boost::units::degree::plane_angle> Degrees;
  typedef boost::units::quantity<si::time> Seconds;
  typedef boost::units::quantity<si::frequency> Hertz;
  typedef boost::units::quantity<si::velocity> Velocity;
}//end Sarry

#endif

