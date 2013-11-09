#ifndef SARRY_ALGORITHM_LINEARALGEBRA_ROI_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_ROI_HPP

#include "Antenna/PerPulseInfo.hpp"
#include "Geo/Elevation.hpp"
#include "Geo/Geo.hpp"

#include <vector>

namespace Sarry
{
  class Chirp;
  class Doi;

  class Roi
  {
  public:
    typedef std::vector<Geo3> GeoList;
    typedef std::vector<std::pair<std::size_t, std::size_t> > IdxPairList;

    enum RoicToUse
    {
      //ROIC which has an entry for every pulse in the physical beamwidth.
      USE_FULL_ROIC,
      //ROIC which has an entry for every pulse in the synthetic beamwidth.
      USE_PARTIAL_ROIC
    };

    Roi(const Doi& doi, const std::vector<PerPulseInfo>& infoList,
        const Chirp& chirp, const std::vector<Geo3>& geoPts,
        Meters resolution, const Elevation& elevation, RoicToUse roicToUse);

    const GeoList& getRoic() const
    { return m_roic; }

    const IdxPairList& getRoiIndexEnvelope() const
    { return m_roiIndices; }

  private:
    /** All pixels of the region of interest closure. */
    GeoList m_roic;

    /** Indexes into m_roic which define the region of interest. */
    IdxPairList m_roiIndices;

  };//end Roi

}//end Sarry

#endif
