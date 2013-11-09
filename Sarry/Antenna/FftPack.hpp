#ifndef SARRY_ANTENNA_FFTPACK_HPP
#define SARRY_ANTENNA_FFTPACK_HPP

#include "Antenna/SarData.hpp"
#include <boost/shared_ptr.hpp>

/* Forward declare the FFTW type. */
extern "C" struct fftw_plan_s;

namespace Sarry
{
  /** A data buffer and a FFT transformation plan (created for the buffer).
   */
  struct FftPack
  {
  private:
    typedef SarData FftPack::*bool_type;
  public:
    typedef boost::shared_ptr<fftw_plan_s> PlanPtr;

    /** planType is either FFTW_FORWARD or FFTW_BACKWARD. */
    FftPack(const SarData& data, int planType);
    FftPack() : data(), pPlan() {}

    ~FftPack();

    operator bool_type() const;

    SarData data;
    PlanPtr pPlan;
  };

}//end Sarry

#endif
