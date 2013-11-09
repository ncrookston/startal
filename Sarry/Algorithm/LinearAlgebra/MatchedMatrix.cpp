
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/MatchedMatrix.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Antenna/Chirp.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Antenna/PerPulseInfoLoader.hpp"
#include "Geo/Geo.hpp"
#include "Geo/getAircraftConverter.hpp"
#include "Geo/toEcef.hpp"
#include "Utility/Constants.hpp"
#include <boost/math/special_functions/sinc.hpp>
#include <boost/range/algorithm/lower_bound.hpp>
#include <boost/units/cmath.hpp>

using Sarry::MatchedMatrix;
using Sarry::Meters;
using Sarry::pi;
using Sarry::Radians;
using Sarry::Seconds;
using boost::math::sinc_pi;

namespace
{
  Seconds eta(std::size_t n, Seconds tau, const Sarry::Chirp& chirp)
  {
    return 1. * n / chirp.getSampleFrequency()
      + chirp.getRangeGateDelay() - tau;
  }

  struct GetIdxLT
  {
    bool operator()(const Sarry::PerPulseInfo& info, std::size_t k)
    { return info.getIdx() < k; }

#ifndef NDEBUG
    //VC8 checks to see if the sequence passed in is actually sorted.
    bool operator()(
      const Sarry::PerPulseInfo& info1, const Sarry::PerPulseInfo& info2)
    { return info1.getIdx() < info2.getIdx(); }
    //VC8 also partially verifies that < is a strict-weak ordering.
    bool operator()(std::size_t k, const Sarry::PerPulseInfo& info)
    { return k < info.getIdx(); }
#endif
  };//end GetIdx

}//end namespace

Sarry::MatchedMatrix::MatchedMatrix(const std::vector<PerPulseInfo>& infoList,
    const Chirp& chirp, const Doi& doi, const Roi& roic)
  : m_infoList(infoList), m_chirp(chirp), m_doi(doi), m_roic(roic),
    m_dataCache(), m_regionCache()
{}

std::complex<Sarry::data_type> MatchedMatrix::operator()(
    std::size_t dataIdx, std::size_t groundIdx) const
{
  Meters L = m_chirp.getAntennaLength();
  Seconds T = m_chirp.getPulseDuration();
  //The compressed pulse duration.
  Seconds compressedT = 1. / m_chirp.getBandWidth();
  BOOST_AUTO(alpha, m_chirp.getBandWidth() / T);
  //NATETODO: No idea what the antenna width is.  This is probably wrong.
  // wrong, however.
  Meters W = m_chirp.getAntennaLength();
  Hertz carrierFreq = m_chirp.getStartTxFrequency();
  Meters wavelength = c / carrierFreq;

  KN s = m_doi.getSamples()[dataIdx];
  bool dataCacheChanged = false;
  if(m_dataCache.idx != s.k)
  {
    dataCacheChanged = true;
    std::vector<PerPulseInfo>::const_iterator it
      = boost::lower_bound(m_infoList, s.k, GetIdxLT());
    assert(it != m_infoList.end() && "Invalid k value specified.");
    const PerPulseInfo& info = *it;
    m_dataCache.idx = s.k;
    m_dataCache.acftPt = toEcef(info.getAcftPosition());
    m_dataCache.toAcs = info.getAntennaCsConverter();
  }

  if(m_regionCache.idx != groundIdx || dataCacheChanged)
  {
    m_regionCache.idx = groundIdx;
    Ecef groundPt = toEcef(m_roic.getRoic()[groundIdx]);
    Meters range = distance(groundPt - m_dataCache.acftPt);
    m_regionCache.tau = range * 2. / c;
    assert(m_dataCache.toAcs && "Data cache invalid.");
    Acs acsPt = (*m_dataCache.toAcs)(groundPt);
    Radians az = atan2(acsPt.y(), acsPt.x());
    Radians el = atan2(acsPt.z(), acsPt.x());

    //Power pattern of the antenna beam.
    double sqrtB = sinc_pi((pi * L / wavelength * sin(az)).value())
      * sinc_pi((pi * W / wavelength * sin(el)).value());
    m_regionCache.b
      = (sqrtB * sqrtB / (range * range * range * range).value());
  }

  Seconds etaVal = eta(s.n, m_regionCache.tau, m_chirp);
  if(etaVal < 0 * si::seconds || etaVal > compressedT)
    return std::complex<data_type>(0., 0.);

  //Magnitude of the matrix term:
  double mag = (m_regionCache.b * (T - abs(etaVal))
    * (sin(pi * alpha * (T - abs(etaVal)) * etaVal)
    / (pi * alpha * etaVal))).value();

  //Phase of the matrix term:
  Radians phase = -pi * 2. * carrierFreq * m_regionCache.tau
    - pi * alpha * T * etaVal;
  return std::complex<data_type>(mag * cos(phase), mag * sin(phase));
}

