#include "Algorithm/Algorithm.hpp"
#include "Algorithm/DataSummary.hpp"
#include "Antenna/Antenna.hpp"
#include "Geo/Elevation.hpp"
#include "Utility/findInstance.hpp"
#include "Utility/loadSharedLibraries.hpp"

#include <boost/program_options.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/units/cmath.hpp>

#include <fstream>

namespace po = boost::program_options;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using boost::units::quantity_cast;
using namespace Sarry;

namespace
{
  void parseArgs(int argc, char* argv[],
    std::string& rAlgorithmType, std::string& rAntennaType,
    std::string& rElevationType, std::string& rSharedLibraryPath)
  {
    po::options_description reqd("Required options");
    reqd.add_options()
      ("algorithm-type", po::value<std::string>(&rAlgorithmType),
        "Algorithm name")
      ("antenna-type", po::value<std::string>(&rAntennaType),
        "Antenna type name")
      ("elevation-type", po::value<std::string>(&rElevationType),
        "Elevation type name")
      ("library-path", po::value<std::string>(&rSharedLibraryPath),
        "Path to directory containing shared libraries");
    std::string cfgFile;
    po::options_description desc("Allowed options");
    desc.add(reqd).add_options()
      ("help", "Show required framework options.  For permitted arguments "
        "to specific algorithms, antennas, or elevation models, supply these "
        "required arguments then add the options help-algorithm, help-antenna, "
        "or help-elevation.")
      ("config-file", po::value<std::string>(&cfgFile)->default_value(
          "sarry_options.cfg"), "specify some or all options via config file");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
      options(desc).allow_unregistered().run(), vm);
    po::notify(vm);

    std::ifstream fin(cfgFile.c_str());
    if(fin)
    {
      po::store(po::parse_config_file(fin, reqd, true), vm);
      po::notify(vm);
    }

    bool isValid = vm.count("algorithm-type") == 1
      && vm.count("antenna-type") == 1
      && vm.count("elevation-type") == 1
      && vm.count("library-path") == 1;

    if(!isValid || vm.count("help"))
      throw std::invalid_argument(boost::lexical_cast<std::string>(desc));
  }
}//end namespace

int main(int argc, char* argv[])
{
  try
  {
    std::string algorithmType, antennaType, elevationType, sharedLibPath;
    parseArgs(argc, argv, algorithmType, antennaType,
              elevationType, sharedLibPath);

    boost::shared_ptr<boost::extensions::type_map>
      pTypes = loadSharedLibraries(sharedLibPath);

    boost::scoped_ptr<Elevation> pElevation(
      findInstance<Elevation>(*pTypes, elevationType, argc, argv));
    if(!pElevation)
    {
      throw std::invalid_argument("Unrecognized elevation type: "
        + elevationType);
    }

    boost::scoped_ptr<Antenna> pAntenna(
      findInstance<Antenna>(*pTypes, antennaType, argc, argv));
    if(!pAntenna)
      throw std::invalid_argument("Unrecognized antenna type: " + antennaType);

    boost::scoped_ptr<Algorithm> pAlgorithm;
    if(algorithmType == "summary")
    {
      pAlgorithm.reset(new DataSummary(argc, argv, *pAntenna, *pElevation));
    }
    else
    {
      pAlgorithm.reset(findInstance<Algorithm>(*pTypes, algorithmType,
          argc, argv, boost::cref(*pAntenna), boost::cref(*pElevation)));
    }
    if(!pAlgorithm)
    {
      throw std::invalid_argument(
          "Unrecognized algorithm specified: " + algorithmType);
    }

    (*pAlgorithm)();
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

