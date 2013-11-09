#ifndef SARRY_ALGORITHM_WRITEKMZ_HPP
#define SARRY_ALGORITHM_WRITEKMZ_HPP

#include "Antenna/SarData.hpp"
#include "Geo/Geo.hpp"

namespace Sarry
{
  void writeKmz(const std::vector<std::complex<data_type> >& image,
                std::size_t width, const std::vector<Geo3>& pts);
}//end Sarry

#endif
