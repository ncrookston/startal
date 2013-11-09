#ifndef SARRY_ALGORITHM_LINEARALGEBRA_REGIONSUMMARY_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_REGIONSUMMARY_HPP

#include "Algorithm/Algorithm.hpp"

namespace Sarry
{
  class Doi;
  class Roi;

  class RegionSummary : public Algorithm
  {
  public:
    RegionSummary(int argc, char** argv, const Antenna& antenna,
                  const Elevation& elevation);

    virtual void operator()() const;

  private:
    const Chirp& m_chirp;
    const PerPulseInfoLoader& m_infoLoader;
    const Elevation& m_elevation;
    std::vector<Geo3> m_pts;
    Meters m_resolution;

  };//end RegionSummary

}//end Sarry

#endif
