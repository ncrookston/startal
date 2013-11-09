#ifndef SARRY_ANTENNA_SARDATA_HPP
#define SARRY_ANTENNA_SARDATA_HPP

#include <boost/shared_ptr.hpp>
#include <complex>
#include <cstddef>
#include <vector>

namespace Sarry
{
  typedef double data_type;

  struct SarData
  {
  private:
    typedef std::size_t SarData::*bool_type;
  public:
    SarData() : offset(0), size(0), pData() {}

    operator bool_type() const { return pData ? &SarData::size : NULL; }

    std::size_t offset;
    std::size_t size;
    boost::shared_ptr<std::vector<std::complex<data_type> > > pData;
  };//end SarData
}//end Sarry

#endif
