#ifndef SARRY_ALGORITHM_LINEARALGEBRA_LINEARALGEBRA_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_LINEARALGEBRA_HPP

#include "Algorithm/Algorithm.hpp"
#include "Antenna/PerPulseInfo.hpp"

namespace Sarry
{
  class Doi;
  class Roi;

  class LinearAlgebra : public Algorithm
  {
  protected:
    LinearAlgebra(int argc, char** argv, const Antenna& antenna,
                  const Elevation& elevation);

    virtual void operator()() const;

    typedef std::pair<std::vector<std::complex<data_type> >, std::size_t>
      AlgResult;
    virtual AlgResult applyAlgorithm(const std::vector<PerPulseInfo>& pulseInfo,
                                     const Doi& doi, const Roi& roic) const = 0;
    const Chirp& m_chirp;
    const PerPulseInfoLoader& m_infoLoader;
    const Elevation& m_elevation;
    std::vector<Geo3> m_pts;
    Meters m_resolution;

  };//end LinearAlgebra

}//end Sarry

#endif
