#ifndef SARRY_ALGORITHM_LINEARALGEBRA_MATCHEDMATRIX_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_MATCHEDMATRIX_HPP

#include "Antenna/SarData.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Geo/Converter.hpp"
#include <boost/optional.hpp>
#include <vector>

namespace Sarry
{
  class Chirp;
  class Doi;
  class Roi;

  class MatchedMatrix
  {
  public:
    MatchedMatrix(const std::vector<PerPulseInfo>& infoList,
      const Chirp& chirp, const Doi& doi, const Roi& roic);

    std::complex<data_type> operator()(
        std::size_t dataIdx, std::size_t groundIdx) const;

  private:
    struct DataCache
    {
      DataCache() : idx((std::numeric_limits<std::size_t>::max)()) {}

      std::size_t idx;
      Ecef acftPt;
      boost::optional<Converter<Ecef, Acs> > toAcs;
    };
    struct RegionCache
    {
      RegionCache() : idx((std::numeric_limits<std::size_t>::max)()) {}

      std::size_t idx;

      double b;
      Seconds tau;
    };

    const std::vector<PerPulseInfo>& m_infoList;
    const Chirp& m_chirp;
    const Doi& m_doi;
    const Roi& m_roic;
    mutable DataCache m_dataCache;
    mutable RegionCache m_regionCache;

  };//end MatchedMatrix

}//end Sarry

#endif
