#ifndef SARRY_UTILITY_LOADSHAREDLIBRARIES_HPP
#define SARRY_UTILITY_LOADSHAREDLIBRARIES_HPP

#include <boost/extension/type_map.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace Sarry
{
  boost::shared_ptr<boost::extensions::type_map>
    loadSharedLibraries(const std::string& sharedLibPath);

}//end Sarry

#endif
