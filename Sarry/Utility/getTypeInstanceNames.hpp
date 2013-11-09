#ifndef SARRY_UTILITY_GETTYPEINSTANCENAMES_HPP
#define SARRY_UTILITY_GETTYPEINSTANCENAMES_HPP

#include "Utility/findInstance.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <vector>

namespace Sarry
{
  template <typename T> inline
  std::vector<std::string> getTypeInstanceNames(
    boost::extensions::type_map& rTypes)
  {
    using boost::adaptors::map_keys;
    typedef typename FactoriesMap<T>::type MapType;
    MapType& factories(rTypes.get());
    std::vector<std::string> names;
    boost::transform(factories | map_keys, std::back_inserter(names),
      boost::phoenix::arg_names::arg1);

    return names;
  }

}//end Sarry

#endif
