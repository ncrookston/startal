#ifndef SARRY_ANTENNA_CHIRP_HPP
#define SARRY_ANTENNA_CHIRP_HPP

#include "Utility/Units.hpp"

namespace Sarry
{
  class Chirp
  {
  public:
    virtual ~Chirp() {}

    //TODO: This is kind-of a mishmash.  We probably don't need ALL of these to
    // be public.
    virtual Hertz getBandWidth() const = 0;

    virtual Hertz getSampleFrequency() const = 0;

    virtual Hertz getStartTxFrequency() const = 0;

    virtual Meters getAntennaLength() const = 0;

    virtual Seconds getPulseDuration() const = 0;

    virtual Seconds getRangeGateDelay() const = 0;

    virtual std::size_t getNumRangeBins() const = 0;

    virtual Radians getHalfBeamwidth() const = 0;

    virtual Hertz getFrequencyShift() const = 0;
  };

}//end Sarry

#endif

