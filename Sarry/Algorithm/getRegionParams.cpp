
#include "getRegionParams.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/units/quantity.hpp>
#include <fstream>

using boost::units::quantity_cast;
namespace po = boost::program_options;

void Sarry::getRegionParams(int argc, char* argv[],
                            Geo2 corners[4], Meters& rResolution)
{
  std::string cfgFile;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help-algorithm", "Show allowed algorithm options")
    ("ul-lat", po::value<double>(&quantity_cast<double&>(corners[0].lat)),
      "upper-left latitude in degrees")
    ("ur-lat", po::value<double>(&quantity_cast<double&>(corners[1].lat)),
      "upper-right latitude in degrees")
    ("lr-lat", po::value<double>(&quantity_cast<double&>(corners[2].lat)),
      "lower-right latitude in degrees")
    ("ll-lat", po::value<double>(&quantity_cast<double&>(corners[3].lat)),
      "lower-left latitude in degrees")
    ("ul-lon", po::value<double>(&quantity_cast<double&>(corners[0].lon)),
      "upper-left longitude in degrees")
    ("ur-lon", po::value<double>(&quantity_cast<double&>(corners[1].lon)),
      "upper-right longitude in degrees")
    ("lr-lon", po::value<double>(&quantity_cast<double&>(corners[2].lon)),
      "lower-right longitude in degrees")
    ("ll-lon", po::value<double>(&quantity_cast<double&>(corners[3].lon)),
      "lower-left longitude in degrees")
    ("resolution", po::value<double>(&quantity_cast<double&>(rResolution)),
      "resolution in meters")
    ("config-file", po::value<std::string>(&cfgFile)->default_value(
        "sarry_options.cfg"), "specify some or all options via config file");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
    options(desc).allow_unregistered().run(), vm);
  po::notify(vm);

  std::ifstream fin(cfgFile.c_str());
  if(fin)
  {
    po::store(po::parse_config_file(fin, desc, true), vm);
    po::notify(vm);
  }

  bool isValid = vm.count("resolution") == 1
    && vm.count("ul-lat") == 1 && vm.count("ul-lon") == 1
    && vm.count("ur-lat") == 1 && vm.count("ur-lon") == 1
    && vm.count("lr-lat") == 1 && vm.count("lr-lon") == 1
    && vm.count("ll-lat") == 1 && vm.count("ll-lon") == 1;

  if(!isValid || vm.count("help-algorithm"))
    throw std::invalid_argument(boost::lexical_cast<std::string>(desc));
}

