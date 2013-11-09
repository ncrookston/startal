#ifndef SARRY_ALGORITHM_LINEARALGEBRA_FHD_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_FHD_HPP

#include "Algorithm/LinearAlgebra/LinearAlgebra.hpp"

namespace Sarry
{
  class FHd : public LinearAlgebra
  {
  public:
    FHd(int argc, char** argv, const Antenna& antenna,
        const Elevation& elevation);

    virtual AlgResult applyAlgorithm(const std::vector<PerPulseInfo>& pulseInfo,
                                     const Doi& doi, const Roi& roic) const;

  };//end FHd

}//end Sarry

#endif
