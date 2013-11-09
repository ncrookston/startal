#include "writeKmz.hpp"
#include "Geo/toKmlString.hpp"
#include "Utility/printToStdout.hpp"
#include "Utility/writeMagImage.hpp"
#include "Utility/ZipFile.hpp"
#include <boost/filesystem.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/tuple/tuple.hpp>

using boost::adaptors::reversed;
namespace fs = boost::filesystem;
namespace px = boost::phoenix;
using px::arg_names::arg1;

void Sarry::writeKmz(
    const std::vector<std::complex<data_type> >& image,
    std::size_t width, const std::vector<Geo3>& pts)
{
  std::cerr << "Size of image: " << image.size() << std::endl;
  std::cerr << "Reported width: " << width << std::endl;
  std::vector<boost::uint8_t> outVals;
  writeMagImg(image, width, outVals);

  boost::gil::gray8c_view_t view = boost::gil::interleaved_view(
    width, outVals.size() / width,
    reinterpret_cast<boost::gil::gray8c_pixel_t*>(&outVals[0]), width);

  fs::path zipDirectory = fs::temp_directory_path() / fs::unique_path();
  fs::create_directory(zipDirectory);
  fs::path imgPath = zipDirectory / "img.jpg";
  boost::gil::jpeg_write_view(imgPath.string(), view, 100);

  std::vector<char> imgVec(fs::file_size(imgPath));
  std::ifstream in(imgPath.string().c_str(), std::ios::binary);
  in.read(&imgVec[0], imgVec.size());
  in.close();
  std::string kml("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  kml += "<kml xmlns=\"http://www.opengis.net/kml/2.2\"\n";
  kml += "     xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n";
  kml += "  <Folder>\n";
  kml += "    <name>Ground Overlay</name>\n";
  kml += "    <GroundOverlay>\n";
  kml += "      <name>SAR overlay</name>\n";
  kml += "      <Icon><href>files/img.jpg</href></Icon>\n";
  kml += "      <gx:LatLonQuad>\n";
  kml += "        <coordinates>\n";
  boost::for_each(pts | reversed,
    px::ref(kml) += px::bind(&toKmlString, arg1) + " ");
  kml += "        </coordinates>\n";
  kml += "      </gx:LatLonQuad>\n";
  kml += "    </GroundOverlay>\n";
  kml += "  </Folder>\n";
  kml += "</kml>\n";

  fs::path zipPath = zipDirectory / "myZip.zip";
  {
    ZipFile zip(zipPath);
    zip.addFile("summary.kml", kml.c_str(), kml.size());
    zip.addFile("files/img.jpg", &imgVec[0], imgVec.size());
  }

  printToStdout(zipPath);

  //fs::remove_all(zipDirectory);
}

