#ifndef SARRY_ANTENNA_PERPULSEINFO_HPP
#define SARRY_ANTENNA_PERPULSEINFO_HPP

#include "SarData.hpp"
#include "Geo/Converter.hpp"
#include "Geo/Euclid.hpp"
#include "Geo/Geo.hpp"
#include "Geo/Orientation.hpp"
#include "Antenna/FftPack.hpp"
#include <boost/function.hpp>

namespace Sarry
{
  class PerPulseInfo
  {
  public:
    typedef boost::function<SarData()> GetRawSarDataCb;
    typedef boost::function<SarData(std::pair<FftPack, FftPack>)>
      GetMatchedSarDataCb;

    PerPulseInfo(std::size_t idx, const Geo3& acftPosition,
      const Converter<Ecef, Acs>& toAcs,
      GetRawSarDataCb getRawData, GetMatchedSarDataCb getMatchedData);

    std::size_t getIdx() const;

    Geo3 getAcftPosition() const;

    Converter<Ecef, Acs> getAntennaCsConverter() const;

    SarData getRawData() const;

    SarData getMatchedData(std::pair<FftPack, FftPack> fftPacks) const;

  private:
    std::size_t m_idx;
    Geo3 m_acftPosition;
    Converter<Ecef, Acs> m_toAcs;
    GetRawSarDataCb m_getRawData;
    GetMatchedSarDataCb m_getMatchedData;
  };//end PerPulseInfo
}//end Sarry

#endif
