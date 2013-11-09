#ifndef SARRY_ALGORITHM_LINEARALGEBRA_WRITEDOI_HPP
#define SARRY_ALGORITHM_LINEARALGEBRA_WRITEDOI_HPP

#include <boost/range/algorithm/for_each.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <fstream>

namespace Sarry
{
  template <typename Range, typename OutIt>
  void writeDoi(const Range& doi, std::size_t nTotal, OutIt outIt)
  {
    namespace px = boost::phoenix;
    using px::arg_names::arg1;
    bool doZero = true;
    std::size_t j = 0;
    boost::for_each(doi,
      (px::ref(doZero) = true,
      px::for_(px::ref(j) = 0, px::ref(j) < px::ref(nTotal), px::ref(j)++)
      [
        px::if_(px::ref(j) == px::bind(&NRange::nStart, arg1)
                || px::ref(j) == px::bind(&NRange::nEnd, arg1))
        [
          px::ref(doZero) = !px::ref(doZero)
        ],

        px::ref(outIt)++ = px::if_else(px::ref(doZero), 0, 255)
      ]
    ));
  }
}//end Sarry

#endif

