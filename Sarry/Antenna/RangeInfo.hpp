#ifndef SARRY_ANTENNA_RANGEINFO_HPP
#define SARRY_ANTENNA_RANGEINFO_HPP

#include "Antenna/Chirp.hpp"
#include "Utility/Constants.hpp"
#include "Utility/round.hpp"
#include "Utility/Units.hpp"
#include <boost/units/cmath.hpp>

namespace Sarry
{
  class RangeInfo
  {
  public:
    explicit RangeInfo(const Chirp& chirp)
      : m_start(), m_delta()
    {
      //We check to see if a certain bit is set in case we emulate sampling
      // a higher frequency with a lower frequency (or something like that).
      // Essentially we multiply by 2 or by 1 -- for the currently-used
      // dataset it's set.
      m_delta = 1. / chirp.getSampleFrequency();

      m_start = chirp.getRangeGateDelay();
    }

    std::size_t toSampleIndex(Meters range) const
    { return round<std::size_t>(toSample(range)); }

    double toSample(Meters range) const
    { return (2.*range / c - m_start) / m_delta; }

    Meters toRange(std::size_t idx) const
    { return (m_start + 1. * idx * m_delta) * c / 2.; }

  private:
    Seconds m_start;
    Seconds m_delta;
  };//end RangeInfo

}//end Sarry

#endif

