#include "Srtm.hpp"

#include "Geo/Euclid.hpp"
#include "Utility/Endian.hpp"
#include "Utility/PhoenixHelpers.hpp"

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/units/cmath.hpp>

namespace fs = boost::filesystem;
namespace io = boost::iostreams;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using namespace Sarry;
using boost::lexical_cast;

static std::string getFileName(const Geo2& pt)
{
  char ns = 'N';
  if(pt.lat < 0 * degrees)
    ns = 'S';
  char ew = 'E';
  if(pt.lon < 0 * degrees)
    ew = 'W';

  std::string hgtName = ns
    + lexical_cast<std::string>(std::abs(static_cast<int>(pt.lat.value())))
    + ew
    + lexical_cast<std::string>(std::abs(static_cast<int>(pt.lon.value())))
    + ".hgt";
  return hgtName;
}

struct SwapBigEndian
{
  template <typename T> 
  T operator()(T t)
  {
    typedef typename boost::make_unsigned<T>::type U_t;
    U_t ut = static_cast<U_t>(t);
    handle(ut, Sarry::Utility::big_endian());
    return static_cast<T>(ut);
  }
};

Srtm::Srtm(const Geo2& pt, const std::string& srtmPathStr)
  : m_size(),
    m_sqrtSize(),
    m_pData(),
    m_loaded(floor(pt.lat), floor(pt.lon))
{
  fs::path srtmPath(srtmPathStr);
  fs::path file = srtmPath / getFileName(m_loaded);

  m_size = static_cast<std::size_t>(fs::file_size(file));
  m_sqrtSize = static_cast<std::size_t>(
          std::sqrt(static_cast<double>(m_size / 2)));
  m_pData.reset(new boost::int16_t[m_size / 2]);

  io::filtering_istream srtm(io::file_source(file.string(), std::ios::binary));
  srtm.read(reinterpret_cast<char*>(m_pData.get()), m_size);

  std::transform(m_pData.get(), m_pData.get() + m_size / 2,
    m_pData.get(), SwapBigEndian());
}

bool Srtm::isInArea(const Geo2& pt) const
{ return floor(pt.lat) == m_loaded.lat && floor(pt.lon) == m_loaded.lon; }

static DPt2 getTrueIndexes(Geo2 pt, std::size_t sqrtSize)
{
  //Reverse the Y axis.
  return DPt2(sqrtSize * (pt.lon - floor(pt.lon)).value(),
              sqrtSize * (1. * degrees - (pt.lat - floor(pt.lat))).value());
}

static SPt2 getMinPt(DPt2 trueIdxs)
{
  std::size_t minX = static_cast<std::size_t>(floor(trueIdxs.x()));
  std::size_t minY = static_cast<std::size_t>(floor(trueIdxs.y()));
  return SPt2(minX, minY);
}

Meters Srtm::operator()(const Geo2& pt) const
{
  assert(floor(pt.lat) == m_loaded.lat && "Out of bounds for SRTM lat data.");
  assert(floor(pt.lon) == m_loaded.lon && "Out of bounds for SRTM lon data.");
  DPt2 trueIndexes = getTrueIndexes(pt, m_sqrtSize);
  SPt2 minPt = getMinPt(trueIndexes);

  bool xInBounds = minPt.x() + 1 < m_sqrtSize;
  bool yInBounds = minPt.y() + 1 < m_sqrtSize;

  boost::int16_t ulE
    = m_pData[minPt.y() * m_sqrtSize + minPt.x()];
  boost::int16_t llE
    = yInBounds ? m_pData[(minPt.y() + 1) * m_sqrtSize + minPt.x()] : ulE;
  boost::int16_t urE
    = xInBounds ? m_pData[minPt.y() * m_sqrtSize + minPt.x() + 1] : ulE;
  boost::int16_t lrE = xInBounds && yInBounds ?
    m_pData[(minPt.y() + 1) * m_sqrtSize + minPt.x() + 1] : ulE;

  if(ulE == INVALID_DATA || urE == INVALID_DATA ||
    lrE == INVALID_DATA || llE == INVALID_DATA)
  {
    assert(!"Need to correctly handle missing data values.");
    throw std::runtime_error("Invalid SRTM data.");
  }

  double pctX = trueIndexes.x() - minPt.x();
  double uInt = ulE * pctX + urE * (1 - pctX);
  double lInt = llE * pctX + lrE * (1 - pctX);

  double pctY = trueIndexes.y() - minPt.y();
  return (uInt * pctY + lInt * (1 - pctY)) * si::meters;
}

