#ifndef SARRY_GEO_ELEVATION_HPP
#define SARRY_GEO_ELEVATION_HPP

#include "Geo/Geo.hpp"
#include <boost/mpl/vector.hpp>

namespace Sarry
{
  /** Interface for retrieving the elevation relative to the ellipsoid
   */
  class Elevation
  {
  public:
    typedef boost::mpl::vector<int, char**> constructor_arguments;
    virtual ~Elevation() {}

    /** Retrieves the elevation (height in meters above the ellipsoid) for the
     *  given lat/lon.
     */
    virtual Meters operator()(const Geo2& pt) const = 0;

  };//end Elevation
}//end Sarry

#endif
