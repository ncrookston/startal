#ifndef SARRY_GEO_SRTM_HPP
#define SARRY_GEO_SRTM_HPP

#include "Geo/Geo.hpp"
#include <boost/cstdint.hpp>
#include <boost/scoped_array.hpp>

namespace Sarry
{
  /** A few notes on Srtm data -- the files refer to the lower-left corner
   *  of the data they contain. Exempli gratia, N41W113.hgt will have data
   *  ranging from 41-42 degrees N and 113-112 degrees W).  The data itself
   *  is in row major order (parallel with latitude lines), starting at
   *  N42W113 and proceeding downward.
   *  The data is also signed 16-bit, gives the height of the geoid and is
   *  stored big-endian.
   */

  class Srtm
  {
  public:
    static const boost::int16_t INVALID_DATA = -32768;

    Srtm(const Geo2& pt, const std::string& srtmPath);

    bool isInArea(const Geo2& pt) const;

    Meters operator()(const Geo2& pt) const;

  private:
    std::size_t m_size;
    boost::scoped_array<boost::int16_t> m_pData;
    std::size_t m_sqrtSize;
    Geo2 m_loaded;
  };

}//end Sarry

#endif

