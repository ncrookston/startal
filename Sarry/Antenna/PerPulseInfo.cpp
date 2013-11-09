
#include "PerPulseInfo.hpp"

using namespace Sarry;

PerPulseInfo::PerPulseInfo(std::size_t idx,
                           const Geo3& acftPosition,
                           const Converter<Ecef, Acs>& toAcs,
                           GetRawSarDataCb getRawData,
                           GetMatchedSarDataCb getMatchedData)
  : m_idx(idx),
    m_acftPosition(acftPosition),
    m_toAcs(toAcs),
    m_getRawData(getRawData),
    m_getMatchedData(getMatchedData)
{}

std::size_t PerPulseInfo::getIdx() const
{ return m_idx; }

Geo3 PerPulseInfo::getAcftPosition() const
{ return m_acftPosition; }

Converter<Ecef, Acs> PerPulseInfo::getAntennaCsConverter() const
{ return m_toAcs; }

SarData PerPulseInfo::getRawData() const
{ return m_getRawData(); }

SarData PerPulseInfo::getMatchedData(std::pair<FftPack, FftPack> fftPacks) const
{ return m_getMatchedData(fftPacks); }

