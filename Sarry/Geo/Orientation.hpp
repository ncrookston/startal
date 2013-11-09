#ifndef SARRY_GEO_ORIENTATION_HPP
#define SARRY_GEO_ORIENTATION_HPP

#include "Utility/Units.hpp"
#include <boost/parameter.hpp>

namespace Sarry
{
  BOOST_PARAMETER_NAME(roll)
  BOOST_PARAMETER_NAME(pitch)
  BOOST_PARAMETER_NAME(yaw)

  namespace Detail
  {
    struct OrientationImpl
    {
      OrientationImpl(const OrientationImpl& rhs)
        : roll(rhs.roll), pitch(rhs.pitch), yaw(rhs.yaw) {}

      template <typename ArgumentPack>
      OrientationImpl(ArgumentPack const& args)
        : roll(args[_roll | 0 * si::radians]),
          pitch(args[_pitch | 0 * si::radians]),
          yaw(args[_yaw | 0 * si::radians])
      {}

      Radians roll;
      Radians pitch;
      Radians yaw;
    };//end OrientationImpl
  }//end Detail

  struct Orientation : Detail::OrientationImpl
  {
    Orientation(const Orientation& rhs)
      : OrientationImpl(static_cast<const OrientationImpl&>(rhs)) {}

    BOOST_PARAMETER_CONSTRUCTOR(Orientation, (Detail::OrientationImpl), tag,
        (optional (roll,*)(pitch,*)(yaw,*)))
  };//end Orientation

}//end Sarry

#endif

