#ifndef SARRY_ALGORITHM_ALGORITHM_HPP
#define SARRY_ALGORITHM_ALGORITHM_HPP

#include "Geo/Geo.hpp"
#include <boost/mpl/vector.hpp>
#include <vector>

namespace Sarry
{
  class Antenna;
  class Chirp;
  struct Orientation;
  class PerPulseInfoLoader;
  class Elevation;

  class Algorithm
  {
  public:
    typedef boost::mpl::vector<int, char**, const Antenna&,
      const Elevation&> constructor_arguments;

    virtual ~Algorithm() {}

    virtual void operator()() const = 0;

  };//end Algorithm

}//end Sarry
#endif
