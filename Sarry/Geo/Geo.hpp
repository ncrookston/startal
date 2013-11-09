#ifndef SARRY_GEO_GEO_HPP
#define SARRY_GEO_GEO_HPP

#include "Utility/Units.hpp"
#include <boost/parameter.hpp>

namespace Sarry
{
  BOOST_PARAMETER_NAME(lat)
  BOOST_PARAMETER_NAME(lon)
  BOOST_PARAMETER_NAME(alt)

  namespace Detail
  {
    struct Geo2Impl
    {
      template <typename ArgumentPack>
      Geo2Impl(ArgumentPack const& args)
        : lat(args[_lat | 0 * degrees]),
          lon(args[_lon | 0 * degrees])
      {}

      Geo2Impl(const Geo2Impl& rhs) : lat(rhs.lat), lon(rhs.lon) {}

      Degrees lat;
      Degrees lon;
    };

    struct Geo3Impl
    {
      Geo3Impl(Degrees lat_, Degrees lon_, Meters alt_)
        : lat(lat_), lon(lon_), alt(alt_) {}

      template <typename ArgumentPack>
      Geo3Impl(ArgumentPack const& args)
        : lat(args[_lat | 0 * degrees]),
          lon(args[_lon | 0 * degrees]),
          alt(args[_alt | 0 * si::meters])
      {}

      Geo3Impl(const Geo3Impl& rhs)
        : lat(rhs.lat), lon(rhs.lon), alt(rhs.alt) {}

      Degrees lat;
      Degrees lon;
      Meters alt;
    };
  }//end Detail

  struct Geo2 : Detail::Geo2Impl
  {
    BOOST_PARAMETER_CONSTRUCTOR(Geo2, (Detail::Geo2Impl), tag,
        (optional (lat,*)(lon,*)))

    //TRICKY: We must cast to the exact type of the copy constructor
    // so the template constructor isn't called instead.
    Geo2(const Geo2& rhs) : Geo2Impl(static_cast<const Geo2Impl&>(rhs)) {}

  };//end Geo2

  inline Geo2 operator-(const Geo2& lhs, const Geo2& rhs)
  { return Geo2(_lat = lhs.lat - rhs.lat, _lon = lhs.lon - rhs.lon); }

  struct Geo3 : Detail::Geo3Impl
  {
    Geo3(Geo2 const& geo2, Meters altitude)
      : Geo3Impl(geo2.lat, geo2.lon, altitude)
    {}

    BOOST_PARAMETER_CONSTRUCTOR(Geo3, (Detail::Geo3Impl), tag,
        (optional (lat,*)(lon,*)(alt,*)))

    //TRICKY: We must cast to the exact type of the copy constructor
    // so the template constructor isn't called instead.
    Geo3(const Geo3& rhs) : Geo3Impl(static_cast<const Geo3Impl&>(rhs)) {}

    Geo2 geo2() const { return Geo2(_lat = lat, _lon = lon); }

  };//end Geo3

  inline Geo3 operator-(const Geo3& lhs, const Geo3& rhs)
  {
    return Geo3(_lat = lhs.lat - rhs.lat, _lon = lhs.lon - rhs.lon,
                _alt = lhs.alt - rhs.alt);
  }

}//end Sarry

#endif

