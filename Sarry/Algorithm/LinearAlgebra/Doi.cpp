
#include "Doi.hpp"
#include "Antenna/Chirp.hpp"
#include "Geo/Euclid.hpp"
#include "Geo/toEcef.hpp"
#include "Antenna/RangeInfo.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Antenna/PerPulseInfoLoader.hpp"
#include "Utility/Units.hpp"
#include "Utility/zipped.hpp"

#include <boost/numeric/conversion/bounds.hpp>
#include <boost/range/adaptor/strided.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/irange.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/units/cmath.hpp>

using Sarry::Doi;
using Sarry::Meters;
namespace num = boost::numeric;
namespace px = boost::phoenix;
using px::arg_names::arg1;
namespace si = Sarry::si;
using boost::adaptors::unpack;
using boost::adaptors::strided;
using boost::adaptors::zip;

namespace
{
  std::size_t getIndexDelta(const Sarry::Chirp& chirp)
  {
    return static_cast<std::size_t>(
        ceil(chirp.getSampleFrequency() / chirp.getBandWidth()));
  }

  struct FindDoi
  {
    FindDoi(const Sarry::Chirp& chirp,
            const std::vector<Sarry::Geo3>& geoPts)
      : m_ecefPts(),
        m_delta(getIndexDelta(chirp)),
        m_chirp(chirp),
        m_rangeInfo(chirp)
    {
      boost::transform(geoPts, std::back_inserter(m_ecefPts),
        boost::bind(&Sarry::toEcef, _1));
    }


    Sarry::NRange operator()(const Sarry::PerPulseInfo& info) const
    {
      //First: find the minimum and maximum range to a geoPoint.
      Sarry::Ecef origin = toEcef(info.getAcftPosition());
      Meters minD = num::bounds<double>::highest() * si::meters;
      Meters maxD = num::bounds<double>::lowest() * si::meters;
      for(std::size_t i = 0, size = m_ecefPts.size(); i < size; ++i)
      {
        Meters range = distance(origin - m_ecefPts[i]);
        minD = (std::min)(minD, range); 
        maxD = (std::max)(maxD, range);
      }
      std::size_t nStart = m_rangeInfo.toSampleIndex(minD) - m_delta;
      std::size_t nEnd = m_rangeInfo.toSampleIndex(maxD) + m_delta;
      Sarry::NRange range = {info.getIdx(), nStart, nEnd};
      return range;
    }

  private:
    std::vector<Sarry::Ecef> m_ecefPts;
    std::size_t m_delta;
    const Sarry::Chirp& m_chirp;
    Sarry::RangeInfo m_rangeInfo;
  };//end FindDoi

  struct BuildKN
  {
    BuildKN(std::size_t k_, const Sarry::SarData& data_)
      : k(k_), data(data_) {}

    Sarry::KN operator()(std::size_t n) const
    { return Sarry::KN(k, n, (*data.pData)[data.offset + n]); }

    std::size_t k;
    Sarry::SarData data;
  };//end BuildKN

  struct ExtractData
  {
    typedef void result_type;

    explicit ExtractData(Doi::SampleList& rOutput, Doi::DataType dataType,
      std::pair<Sarry::FftPack, Sarry::FftPack> fftPacks)
      : m_rOutput(rOutput), m_dataType(dataType), m_fftPacks(fftPacks)
    {}
  
    void operator()(const Sarry::NRange& idx, const Sarry::PerPulseInfo& info)
    {
      Sarry::SarData data;
      if(m_dataType == Doi::MATCH_FILTER_DATA)
        data = info.getMatchedData(m_fftPacks);
      else
        data = info.getRawData();

      boost::transform(boost::irange(idx.nStart, idx.nEnd + 1),
        std::back_inserter(m_rOutput), BuildKN(idx.k, data));
    }

    Doi::SampleList& m_rOutput;
    Doi::DataType m_dataType;
    std::pair<Sarry::FftPack, Sarry::FftPack> m_fftPacks;
  };//end ExtractData
}//end namespace

Doi::Doi(const std::vector<Sarry::PerPulseInfo>& infoList,
         const Sarry::Chirp& chirp, const std::vector<Sarry::Geo3>& geoPts,
         Doi::DataType dataType,
         std::pair<Sarry::FftPack, Sarry::FftPack> fftPacks)
  : m_azList(), m_idxToKN()
{
  boost::transform(infoList, std::back_inserter(m_azList),
    FindDoi(chirp, geoPts));

  boost::for_each(zip(m_azList, infoList),
    unpack(ExtractData(m_idxToKN, dataType, fftPacks)));
}

