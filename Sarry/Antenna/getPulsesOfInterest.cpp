#include "getHalfFov.hpp"
#include "getPulsesOfInterest.hpp"
#include "Geo/getAircraftConverter.hpp"
#include "Geo/toEcef.hpp"
#include "PerPulseInfoLoader.hpp"
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/spirit/home/phoenix.hpp>

namespace num = boost::numeric;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using px::arg_names::arg2;

std::vector<Sarry::PerPulseInfo> Sarry::getPulsesOfInterest(
    const PerPulseInfoLoader& infoLoader, const Chirp& chirp,
    const std::vector<Geo3>& geoPts)
{
  //For each index, see if our selected geo points are within the limits.
  std::vector<Ecef> ecefPts;
  boost::transform(geoPts, std::back_inserter(ecefPts),
      px::bind(&toEcef, arg1));
  Radians antennaHBW = getHalfFov(chirp);

  std::vector<PerPulseInfo> outList;
  for(std::size_t idx = 0; infoLoader.isValid(idx); ++idx)
  {
    PerPulseInfo info = infoLoader[idx];
    BOOST_AUTO(toAcs, info.getAntennaCsConverter());
    Ecef acftPt = toEcef(info.getAcftPosition());

    Radians minFOV = num::bounds<double>::highest() * si::radians;
    Radians maxFOV = num::bounds<double>::lowest() * si::radians;
    for(std::size_t pdx = 0, sz = ecefPts.size(); pdx < sz; ++pdx)
    {
      Meters range = distance(ecefPts[pdx] - acftPt);

      Acs acsPt = toAcs(ecefPts[pdx]);
      Radians azToPt = atan2(acsPt.y(), acsPt.x());

      minFOV = (std::min)(minFOV, azToPt);
      maxFOV = (std::max)(maxFOV, azToPt);
    }

    const Radians zero = 0. * si::radians;
    //If the antenna is between extreme points.
    // or if it's sufficiently close to the extreme points,
    if((minFOV < zero && maxFOV > zero)
      || abs(minFOV) < antennaHBW || abs(maxFOV) < antennaHBW)
    {
      outList.push_back(info);
    }
  }

  //outList is sorted with regards to getIdx().
  return outList;
}
