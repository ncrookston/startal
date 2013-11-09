#include "LinearAlgebra.hpp"
#include "Algorithm/getRegionParams.hpp"
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Algorithm/writeKmz.hpp"
#include "Antenna/Antenna.hpp"
#include "Antenna/getPulsesOfInterest.hpp"
#include "Antenna/PerPulseInfoLoader.hpp"
#include "Geo/Elevation.hpp"
#include "Geo/GeoToIndex.hpp"
#include <boost/range/algorithm/transform.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>

namespace px = boost::phoenix;
using px::arg_names::arg1;
using namespace Sarry;


Sarry::LinearAlgebra::LinearAlgebra(int argc, char** argv,
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

void Sarry::LinearAlgebra::operator()() const
{
  AlgResult::first_type gVals;
  std::size_t elVals;
  std::pair<FftPack, FftPack> fftPacks = m_infoLoader.getMatchedFilterPacks();
  {
    //Aircraft body angles and locations indexed by pulse.
    const std::vector<PerPulseInfo> pulseInfo
      = getPulsesOfInterest(m_infoLoader, m_chirp, m_pts);

    std::clog << "Determining data of interest. . ." << std::endl;
    //The data of interest.
    Doi doi(pulseInfo, m_chirp, m_pts, Doi::MATCH_FILTER_DATA, fftPacks);

    std::clog << "Determining region of interest closure. . ." << std::endl;
    //The region of interest closure (with roi accessible).
    Roi roic(doi, pulseInfo, m_chirp, m_pts, m_resolution, m_elevation,
      Roi::USE_PARTIAL_ROIC);

    boost::tie(gVals, elVals) = applyAlgorithm(pulseInfo, doi, roic);
  }

  writeKmz(gVals, elVals, m_pts);
}

