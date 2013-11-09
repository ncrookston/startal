#ifndef SARRY_ALGORITHM_LINEARALGEBRA_MAXIMUMLIKELIHOOD_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_MAXIMUMLIKELIHOOD_HPP

#include "Algorithm/LinearAlgebra/LinearAlgebra.hpp"

namespace Sarry
{
  class MaximumLikelihood : public LinearAlgebra
  {
  public:
    MaximumLikelihood(int argc, char** argv, const Antenna& antenna,
                      const Elevation& elevation);

    virtual AlgResult applyAlgorithm(const std::vector<PerPulseInfo>& pulseInfo,
                                     const Doi& doi, const Roi& roic) const;

  };//end MaximumLikelihood

}//end Sarry

#endif
