#ifndef SARRY_UTILITY_WRITEMAGIMAGE_HPP
#define SARRY_UTILITY_WRITEMAGIMAGE_HPP

#include "Antenna/SarData.hpp"
#include <boost/cstdint.hpp>
#include <complex>
#include <vector>

namespace Sarry
{
  void writeMagImg(const std::vector<std::complex<data_type> >& g,
      std::size_t width, std::vector<boost::uint8_t>& rGChar);

}//end Sarry

#endif

