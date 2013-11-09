
#pragma warning(disable: 4244)
#include "Algorithm/getRegionParams.hpp"
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/RegionSummary.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Algorithm/LinearAlgebra/writeDoi.hpp"
#include "Antenna/Antenna.hpp"
#include "Antenna/Chirp.hpp"
#include "Antenna/getPulsesOfInterest.hpp"
#include "Antenna/PerPulseInfoLoader.hpp"
#include "Geo/Elevation.hpp"
#include "Geo/writePointList.hpp"
#include "Utility/printToStdout.hpp"
#include "Utility/ZipFile.hpp"
#include <boost/filesystem.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/tuple/tuple.hpp>

namespace fs = boost::filesystem;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using boost::adaptors::sliced;

Sarry::RegionSummary::RegionSummary(int argc, char** argv,
      const Antenna& antenna, const Elevation& elevation)
  : m_chirp(antenna.getChirp()),
    m_infoLoader(antenna.getPulseLoader()),
    m_elevation(elevation),
    m_pts(4),
    m_resolution()
{
  Geo2 corners[4];
  getRegionParams(argc, argv, corners, m_resolution);

  boost::transform(corners, m_pts.begin(), px::construct<Geo3>(arg1,
      px::bind(&Sarry::Elevation::operator(), &elevation, arg1)));
}

void Sarry::RegionSummary::operator()() const
{
  std::pair<FftPack, FftPack> fftPacks = m_infoLoader.getMatchedFilterPacks();

  //Aircraft body angles and locations indexed by pulse.
  const std::vector<PerPulseInfo> pulseInfo
    = getPulsesOfInterest(m_infoLoader, m_chirp, m_pts);

  std::clog << "Determining data of interest. . ." << std::endl;
  //The data of interest.
  Doi doi(pulseInfo, m_chirp, m_pts, Doi::MATCH_FILTER_DATA, fftPacks);

  std::clog << "Determining region of interest closure. . ." << std::endl;
  //The region of interest closure (with roi accessible).
  Roi roic(doi, pulseInfo, m_chirp, m_pts, m_resolution, m_elevation,
    Roi::USE_FULL_ROIC);

  std::vector<boost::uint8_t> doiImg;
  writeDoi(doi.getEnvelope(), m_chirp.getNumRangeBins(),
    std::back_inserter(doiImg));
  std::size_t width = m_chirp.getNumRangeBins();
  boost::gil::gray8c_view_t view = boost::gil::interleaved_view(
    width, doiImg.size() / width,
    reinterpret_cast<boost::gil::gray8c_pixel_t*>(&doiImg[0]), width);

  fs::path zipDirectory = fs::temp_directory_path() / fs::unique_path();
  fs::create_directory(zipDirectory);
  fs::path imgPath = zipDirectory / "roi.jpg";

  boost::gil::gray8_image_t newView(512, 512 * view.height() / view.width());
  boost::gil::resize_view(view,
    boost::gil::view(newView), boost::gil::bilinear_sampler());
  jpeg_write_view(imgPath.string(), boost::gil::view(newView), 100);

  std::vector<char> imgVec(fs::file_size(imgPath));
  std::ifstream in(imgPath.string().c_str(), std::ios::binary);
  in.read(&imgVec[0], imgVec.size());
  in.close();

  const Roi::GeoList& roicPts = roic.getRoic();
  std::vector<Geo3> roiPts, nonRoiPts;
  const Roi::IdxPairList& idxEnvelope = roic.getRoiIndexEnvelope();
  std::size_t lastEnd = 0;
  for(std::size_t y = 0; y < idxEnvelope.size(); ++y)
  {
    std::size_t xStart, xEnd;
    boost::tie(xStart, xEnd) = idxEnvelope[y];
    boost::copy(roicPts | sliced(xStart, xEnd), std::back_inserter(roiPts));
    boost::copy(roicPts | sliced(lastEnd, xStart),
      std::back_inserter(nonRoiPts));
    lastEnd = xEnd;
  }
  boost::copy(roicPts | sliced(lastEnd, roicPts.size()),
    std::back_inserter(nonRoiPts));

  std::string out;
  out += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out += "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
  out += "<Document>\n";
  out += "  <Style id=\"PT1\">\n";
  out += "    <BalloonStyle>\n";
  out += "      <text><![CDATA[<img src=\"files/roi.jpg\" />]]></text>\n";
  out += "    </BalloonStyle>\n";
  out += "    <IconStyle>\n";
  out += "      <color>ff00ccff</color><scale>0.5</scale>\n";
  out += "      <Icon><href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href></Icon>\n";
  out += "    </IconStyle>\n";
  out += "    <LabelStyle><scale>0</scale></LabelStyle>\n";
  out += "  </Style>\n";
  out += "  <Style id=\"PT2\">\n";
  out += "    <IconStyle>\n";
  out += "      <color>ffff0000</color><scale>0.5</scale>\n";
  out += "      <Icon><href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href></Icon>\n";
  out += "    </IconStyle>\n";
  out += "    <LabelStyle><scale>0</scale></LabelStyle>\n";
  out += "  </Style>\n";

  out += writePointList("ROI", roiPts, "PT1");
  out += writePointList("ROIC", nonRoiPts, "PT2");

  out += "</Document>\n";
  out += "</kml>\n";

  fs::path zipPath = zipDirectory / "myZip.zip";
  {
    ZipFile zip(zipPath);
    zip.addFile("summary.kml", out.c_str(), out.size());
    zip.addFile("files/roi.jpg", &imgVec[0], imgVec.size());
  }

  printToStdout(zipPath);

}

