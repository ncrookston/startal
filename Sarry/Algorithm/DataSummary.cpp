
#include "DataSummary.hpp"
#include "Antenna/Antenna.hpp"
#include "Antenna/Chirp.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Antenna/PerPulseInfoLoader.hpp"
#include "Geo/ToGround.hpp"
#include "Geo/toKmlString.hpp"
#include "Utility/Constants.hpp"
#include "Utility/printToStdout.hpp"
#include "Utility/ZipFile.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/range/algorithm/for_each.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace
{
  struct PrintPlacemark
  {
    explicit PrintPlacemark(std::string& rStr)
      : m_rStr(rStr)
    {}

    void operator()(const Sarry::Geo3& pt)
    {
      m_rStr += "      <Placemark>\n";
      m_rStr += "        <styleUrl>#GroundPt</styleUrl>\n";
      m_rStr += "        <Point>\n";
      m_rStr += "          <coordinates>"+toKmlString(pt)+"</coordinates>\n";
      m_rStr += "        </Point>\n";
      m_rStr += "      </Placemark>\n";
    }

    std::string& m_rStr;
  };//end PrintPlacemark

}//end namespace
Sarry::DataSummary::DataSummary(int argc, char** argv, const Antenna& antenna,
        const Elevation& elevation)
  : m_antenna(antenna), m_elevation(elevation)
{
  //If the user specifically asks for this algorithm's expected arguments,
  // we don't want to continue to execute the algorithm.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help-algorithm", "Show allowed algorithm options");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
    options(desc).allow_unregistered().run(), vm);
  po::notify(vm);

  if(vm.count("help-algorithm") == 1)
    throw std::invalid_argument("No additional summary arguments.");
}

//virtual method
void Sarry::DataSummary::operator()() const
{
  const PerPulseInfoLoader& loader = m_antenna.getPulseLoader();
  const Chirp& chirp = m_antenna.getChirp();
  Meters near = c * chirp.getRangeGateDelay() / 2.;
  Meters far = near
    + 1. * chirp.getNumRangeBins() * c / (2. * chirp.getSampleFrequency());
  std::vector<Geo3> nearPts, farPts;

  std::string kml("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  kml += "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
  kml += "  <Document>\n";
  kml += "    <Style id=\"curtain\">\n";
  kml += "      <LineStyle>\n";
  kml += "        <color>7fff00ff</color>\n";
  kml += "        <width>4</width>\n";
  kml += "      </LineStyle>\n";
  kml += "      <PolyStyle>\n";
  kml += "        <color>7f00ff00</color>\n";
  kml += "      </PolyStyle>\n";
  kml += "    </Style>\n";
  kml += "    <Style id=\"GroundPt\">\n";
  kml += "      <IconStyle>\n";
  kml += "        <color>ffff9600</color>\n";
  kml += "        <scale>0.5</scale>\n";
  kml += "        <Icon>\n";
  kml += "          <href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href>\n";
  kml += "        </Icon>\n";
  kml += "      </IconStyle>\n";
  kml += "      <LabelStyle>\n";
  kml += "        <scale>0</scale>\n";
  kml += "      </LabelStyle>\n";
  kml += "    </Style>\n";
  kml += "    <Folder>\n";
  kml += "      <name>Plane Locations</name>\n";
  kml += "      <description>Collection Flight Track</description>\n";
  kml += "      <Placemark>\n";
  kml += "        <name>Aircraft Collection Flight Path</name>\n";
  kml += "        <visibility>1</visibility>\n";
  kml += "        <styleUrl>#curtain</styleUrl>\n";
  kml += "        <LineString>\n";
  kml += "          <extrude>1</extrude>\n";
  kml += "          <tessellate>1</tessellate>\n";
  kml += "          <altitudeMode>absolute</altitudeMode>\n";
  kml += "          <coordinates>\n";

  for(std::size_t idx = 0; loader.isValid(idx); idx += 1000)
  {
    const PerPulseInfo& info = loader[idx];
    Geo3 pt =  info.getAcftPosition();

    ToGround toGround(pt, info.getAntennaCsConverter(), m_elevation,
      ToGround::RETURN_CLOSEST);
    nearPts.push_back(toGround(near));
    farPts.push_back(toGround(far));

    kml += toKmlString(pt) + '\n';
  }
  kml += "          </coordinates>\n";
  kml += "        </LineString>\n";
  kml += "      </Placemark>\n";
  kml += "    </Folder>\n";
  kml += "    <Folder>\n";
  boost::for_each(nearPts, PrintPlacemark(kml));
  boost::for_each(farPts, PrintPlacemark(kml));
  kml += "    </Folder>\n";
  kml += "  </Document>\n";
  kml += "</kml>\n";

  fs::path zipDirectory = fs::temp_directory_path() / fs::unique_path();
  fs::create_directory(zipDirectory);
  fs::path zipPath = zipDirectory / "myZip.zip";
  {
    ZipFile zip(zipPath);
    zip.addFile("summary.kml", kml.c_str(), kml.size());
  }

  printToStdout(zipPath);

  fs::remove_all(zipDirectory);
}

