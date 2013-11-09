
#include "SrtmElevation.hpp"
#include "Srtm/Srtm.hpp"
#include "Utility/findInstance.hpp"
#include "GeographicLib/Geoid.hpp"
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <fstream>


using namespace Sarry;
namespace ext = boost::extensions;
namespace po = boost::program_options;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using boost::make_shared;

namespace
{
  void parseArgs(int argc, char** argv, std::string& rSrtmPath,
                 std::string& rGeoidType, std::string& rGeoidPath)
  {
    std::string cfgFile;
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help-elevation", "produce elevation help message")
      ("config-file", po::value<std::string>(&cfgFile)->default_value(
          "sarry_options.cfg"), "specify some or all options via config file")
      ("srtm-location", po::value<std::string>(&rSrtmPath)->default_value(
          "C:/data/SRTM/"), "Path to SRTM .hgt files")
      //SRTM data is relative to the 96 EGM, so we generally won't use the
      // higher-res 2008 data -- egm2008-1
      ("geoid-type", po::value<std::string>(&rGeoidType)->default_value(
          "egm96-5"), "Reference geoid type")
      ("geoid-path", po::value<std::string>(&rGeoidPath)->default_value(
          "C:/data/geoids"), "Path to precomputed geoid heights");

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

    if(vm.count("help-elevation") == 1)
      throw std::invalid_argument(boost::lexical_cast<std::string>(desc));
  }
}//end namespace

SrtmElevation::SrtmElevation(int argc, char** argv)
  : m_srtms(),
    m_pGeoid(),
    m_srtmPath()
{
  std::string geoidType, geoidPath;
  parseArgs(argc, argv, m_srtmPath, geoidType, geoidPath);
  m_pGeoid.reset(new GeographicLib::Geoid(geoidType, geoidPath));
}

SrtmElevation::~SrtmElevation()
{}

Meters SrtmElevation::operator()(const Geo2& pt) const
{
  //Find the correct SRTM object.
  SrtmList::iterator it = boost::find_if(m_srtms, (arg1->*&Srtm::isInArea)(pt));
  boost::shared_ptr<const Srtm> pSrtm;
  //If not found, load it.
  //NATETODO: Handle out of memory by making m_srtms a MRU list.
  if(it == m_srtms.end())
  {
    m_srtms.push_back(make_shared<Srtm>(pt, m_srtmPath));
    pSrtm = m_srtms.back();
  }
  else
    pSrtm = *it;

  return (*pSrtm)(pt)
    + (*m_pGeoid)(pt.lat.value(), pt.lon.value()) * si::meters;
}

BOOST_EXTENSION_TYPE_MAP_FUNCTION
{
  FactoriesMap<Sarry::Elevation>::type& elevation_factories(types.get());
  elevation_factories["srtm"].set<Sarry::SrtmElevation>(); 
}

