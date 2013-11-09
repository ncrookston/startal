#ifndef SARRY_GEO_TOKMLSTRING_HPP
#define SARRY_GEO_TOKMLSTRING_HPP

#include "Geo/Geo.hpp"
#include <boost/lexical_cast.hpp>

namespace Sarry
{
  inline std::string toKmlString(const Geo3& pt)
  {
    return boost::lexical_cast<std::string>(pt.lon.value())
      + ',' + boost::lexical_cast<std::string>(pt.lat.value())
      + ',' + boost::lexical_cast<std::string>(pt.alt.value());
  }
}//end Sarry

#endif
