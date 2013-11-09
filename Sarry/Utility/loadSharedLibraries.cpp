#include "loadSharedLibraries.hpp"
#include <boost/extension/shared_library.hpp>
#include <boost/extension/type_map.hpp>
#include <boost/filesystem.hpp>

namespace ext = boost::extensions;
namespace fs = boost::filesystem;

boost::shared_ptr<ext::type_map>
    Sarry::loadSharedLibraries(const std::string& sharedLibPath)
{
  boost::shared_ptr<ext::type_map> pTypes(new ext::type_map);
  fs::path sharedPath(sharedLibPath);
  for(fs::directory_iterator it = fs::directory_iterator(sharedPath),
      end = fs::directory_iterator(); it != end; ++it)
  {
    ext::shared_library lib(it->path().string());
    if(lib.open())
      lib.call(*pTypes);
  }

  return pTypes;
}

