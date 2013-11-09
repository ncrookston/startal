#include "Algorithm/Algorithm.hpp"
#include "Antenna/Antenna.hpp"
#include "Geo/Elevation.hpp"
#include "Utility/loadSharedLibraries.hpp"
#include "Utility/getTypeInstanceNames.hpp"

#include <boost/program_options.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/spirit/home/phoenix.hpp>

namespace po = boost::program_options;
namespace px = boost::phoenix;
using px::arg_names::arg1;

namespace
{
  void parseArgs(int argc, char* argv[],
    std::string& rFactoryType, std::string& rSharedLibraryPath)
  {
    po::options_description reqd("Required options");
    reqd.add_options()
      ("type", po::value<std::string>(&rFactoryType),
        "Factory name (e.g., Antenna or Algorithm)")
      ("library-path", po::value<std::string>(&rSharedLibraryPath),
        "Path to directory containing shared libraries");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(reqd).run(), vm);
    po::notify(vm);

    bool isValid = vm.count("type") == 1 && vm.count("library-path") == 1;

    if(!isValid)
      throw std::invalid_argument(boost::lexical_cast<std::string>(reqd));
  }
}//end namespace

int main(int argc, char* argv[])
{
  try
  {
    std::string type, sharedLibPath;
    parseArgs(argc, argv, type, sharedLibPath);

    boost::shared_ptr<boost::extensions::type_map>
      pTypes = Sarry::loadSharedLibraries(sharedLibPath);

    std::vector<std::string> types;
    if(type == "elevation")
      types = Sarry::getTypeInstanceNames<Sarry::Elevation>(*pTypes);
    else if(type == "antenna")
      types = Sarry::getTypeInstanceNames<Sarry::Antenna>(*pTypes);
    else if(type == "algorithm")
      types = Sarry::getTypeInstanceNames<Sarry::Algorithm>(*pTypes);

    boost::for_each(types, std::cout << arg1 << std::endl);
  }
  catch(const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
  }

  return 0;
}

