#ifndef SARRY_GEO_POINTING_HPP
#define SARRY_GEO_POINTING_HPP

#include "Utility/Units.hpp"
#include <boost/parameter.hpp>

namespace Sarry
{
  BOOST_PARAMETER_NAME(azimuth)
  BOOST_PARAMETER_NAME(elevation)

  namespace Detail
  {
    struct PointingImpl
    {
      template <typename ArgumentPack>
      PointingImpl(ArgumentPack const& args)
        : azimuth(args[_azimuth | 0 * si::radians]),
          elevation(args[_elevation | 0 * si::radians])
      {}

      Radians azimuth;
      Radians elevation;
    };//end PointingImpl
  }//end Detail

  struct Pointing : Detail::PointingImpl
  {
    BOOST_PARAMETER_CONSTRUCTOR(Pointing, (Detail::PointingImpl), tag,
        (optional (azimuth,*)(elevation,*)))
  };//end Pointing

}//end Sarry

#endif

