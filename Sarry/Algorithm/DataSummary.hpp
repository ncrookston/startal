#ifndef SARRY_ALGORITHM_DATASUMMARY_HPP
#define SARRY_ALGORITHM_DATASUMMARY_HPP

#include "Algorithm/Algorithm.hpp"

namespace Sarry
{
  class DataSummary : public Algorithm
  {
  public:
    DataSummary(int argc, char** argv, const Antenna& antenna,
        const Elevation& elevation);

    virtual void operator()() const;

  private:
    const Antenna& m_antenna;
    const Elevation& m_elevation;

  };//end DataSummary

}//end Sarry
#endif
