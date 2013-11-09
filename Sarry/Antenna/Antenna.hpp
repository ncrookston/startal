#ifndef SARRY_ANTENNA_ANTENNA_HPP
#define SARRY_ANTENNA_ANTENNA_HPP

#include <boost/mpl/vector.hpp>

namespace Sarry
{
  class Chirp;
  struct Orientation;
  class PerPulseInfoLoader;

  class Antenna
  {
  public:
    typedef boost::mpl::vector<int, char**> constructor_arguments;

    virtual ~Antenna()
    {}

    virtual const Chirp& getChirp() const = 0;

    virtual const PerPulseInfoLoader& getPulseLoader() const = 0;

  };//end Antenna
}//end Sarry

#endif
