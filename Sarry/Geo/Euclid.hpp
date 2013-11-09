#ifndef SARRY_GEO_EUCLID_HPP
#define SARRY_GEO_EUCLID_HPP

#include "Geo/Vec.hpp"
#include "Utility/Units.hpp"

namespace Sarry
{
  struct euclid_tag {};

  typedef Vec<euclid_tag, std::size_t, 2> SPt2;
  typedef Vec<euclid_tag, std::size_t, 3> SPt3;
  
  typedef Vec<euclid_tag, int, 2> IPt2;
  typedef Vec<euclid_tag, int, 3> IPt3;

  typedef Vec<euclid_tag, double, 2> DPt2;
  typedef Vec<euclid_tag, double, 3> DPt3;

  typedef Vec<euclid_tag, Meters, 2> MPt2;
  typedef Vec<euclid_tag, Meters, 3> MPt3;

  struct earth_centered_earth_fixed_tag : euclid_tag {};

  typedef Vec<earth_centered_earth_fixed_tag, Meters, 3> Ecef;

  struct north_west_up_coordinate_system_tag : euclid_tag {};
  //This system is a right-handed system where z is orthogonal to a local
  // tangent plane, x is in the tangent plane and in the plane formed by
  // the z vector and the north vector, and y is orthogonal to both.
  typedef Vec<north_west_up_coordinate_system_tag, Meters, 3> Nwu;

  struct aircraft_coordinate_system_tag : euclid_tag {};
  //This system is right-handed where x is positive out the nose of the
  // aircraft, z is up relative to the plane of the aircraft, and y is
  // orthogonal to both.
  typedef Vec<aircraft_coordinate_system_tag, Meters, 3> AcftCs;

  //This system is relative to the antenna -- x is the antenna boresight, z
  // is up and y is the cross product of x & z.  This is a right-handed
  // coordinate system.
  struct antenna_coordinate_system_tag : euclid_tag {};
  typedef Vec<antenna_coordinate_system_tag, Meters, 3> Acs;

  //Cross product is only defined for 3 dimensions.
  template <typename TAG, typename T>
  Vec<TAG, T, 3> cross(const Vec<TAG, T, 3>& lhs, const Vec<TAG, T, 3>& rhs,
    typename boost::enable_if<boost::is_base_of<euclid_tag, TAG> >::type* = 0)
  {
    //TODO: The dimensions are wrong for this type.  I think we need to divide
    // by the magnitude.
    return Vec<TAG, T, 3>(
      lhs.y() * rhs.z() - lhs.z() * rhs.y(),
      lhs.z() * rhs.x() - lhs.x() * rhs.z(),
      lhs.x() * rhs.y() - lhs.y() * rhs.x());
  }

}//end Sarry

#endif

