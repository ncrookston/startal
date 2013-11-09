#ifndef SARRY_ALGORITHM_LINEARALGEBRA_FINDDOI_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_FINDDOI_HPP

#include "Antenna/PerPulseInfo.hpp"
#include "Antenna/FftPack.hpp"
#include "Antenna/SarData.hpp"
#include "Geo/Geo.hpp"

#include <boost/bind.hpp>
#include <complex>
#include <deque>
#include <vector>

namespace Sarry
{
  class Chirp;
  class PerPulseInfoLoader;

  struct NRange
  {
    std::size_t k;
    std::size_t nStart;
    std::size_t nEnd;
  };//end NRange

  struct KN
  {
    KN(std::size_t k_, std::size_t n_, const std::complex<data_type>& data_)
      : k(k_), n(n_), data(data_) {}

    std::size_t k, n;

    std::complex<data_type> data;
  };//end KN

  class Doi
  {
    typedef std::vector<NRange> AzList;
  public:
    enum DataType
    {
      MATCH_FILTER_DATA,
      RAW_DATA,
    };//end DataType

    typedef AzList::const_iterator ConstAzIt;
    typedef std::deque<KN> SampleList;

    Doi(const std::vector<PerPulseInfo>& infoList, const Chirp& chirp,
      const std::vector<Geo3>& geoPts, DataType dataType,
      std::pair<FftPack, FftPack> fftPacks);

    std::pair<ConstAzIt, ConstAzIt> getEnvelope() const
    { return std::pair<ConstAzIt,ConstAzIt>(m_azList.begin(),m_azList.end()); }

    const SampleList& getSamples() const
    { return m_idxToKN; }

  private:

    AzList m_azList;

    SampleList m_idxToKN;
  };//end Doi

}//end Sarry

#endif

