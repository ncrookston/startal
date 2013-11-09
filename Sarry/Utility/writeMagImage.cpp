
#include "writeMagImage.hpp"
#include "Utility/PhoenixHelpers.hpp"
#include <boost/algorithm/minmax_element.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/transform.hpp>
#define BOOST_TYPEOF_SILENT
#include <boost/typeof/typeof.hpp>

using boost::adaptors::transformed;
namespace px = boost::phoenix;
using px::arg_names::arg1;

void Sarry::writeMagImg(const std::vector<std::complex<data_type> >& g,
      std::size_t width, std::vector<boost::uint8_t>& rGChar)
{
  std::vector<data_type> gMag;
  boost::copy(g | transformed(px::sqrt(px::abs(arg1))),
    std::back_inserter(gMag));
  BOOST_AUTO(iters, boost::minmax_element(gMag.begin(), gMag.end()));
  assert(iters.first != gMag.end() && iters.second != gMag.end()
      && "Apparently g is empty.  Unable to write image.");

  double minV = *iters.first;
  double maxV = *iters.second;
  boost::transform(gMag, std::back_inserter(rGChar),
    px::static_cast_<boost::uint8_t>(255. * (arg1 - minV) / (maxV - minV)));
}

