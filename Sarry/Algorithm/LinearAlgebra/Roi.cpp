
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Antenna/Chirp.hpp"
#include "Antenna/RangeInfo.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Geo/Elevation.hpp"
#include "Geo/GeoToIndex.hpp"
#include "Geo/ToGround.hpp"
#include "Utility/Units.hpp"
#include "Utility/zipped.hpp"

#include <boost/range/adaptor/strided.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <map>

using Sarry::Chirp;
using Sarry::Elevation;
using Sarry::Geo2;
using Sarry::Geo3;
using Sarry::PerPulseInfo;
using Sarry::Roi;
using Sarry::ToGround;

using boost::adaptors::unpack;
using boost::adaptors::strided;
using boost::adaptors::zip;

namespace
{
  struct FindRoiClosure
  {
    typedef std::pair<Geo3, Geo3> result_type;

    FindRoiClosure(const Chirp& chirp, const Elevation& elevation)
      : m_chirp(chirp), m_elevation(elevation), m_rangeInfo(chirp)
    {}

    result_type operator()(const Sarry::NRange& sample,
      const PerPulseInfo& info) const
    {
      Sarry::Meters nearRange = m_rangeInfo.toRange(sample.nStart);
      Sarry::Meters farRange = m_rangeInfo.toRange(sample.nEnd);

      ToGround toGround(info.getAcftPosition(), info.getAntennaCsConverter(),
        m_elevation, ToGround::RETURN_CLOSEST);

      return std::make_pair(toGround(nearRange), toGround(farRange));
    }

    const Chirp& m_chirp;
    const Elevation& m_elevation;
    const Sarry::RangeInfo m_rangeInfo;
  };//end FindRoiClosure

  typedef std::map<int, std::pair<int, int> > IndexList;
  struct UpdateIdxs
  {
    UpdateIdxs(IndexList& rNeededIdxs, const Sarry::GeoToIndex& cvtr)
      : m_rNeededIdxs(rNeededIdxs), m_cvtr(cvtr)
    {}

    void operator()(const std::pair<Geo3, Geo3>& pts)
    {
      (*this)(pts.first.geo2());
      (*this)(pts.second.geo2());
    }

    void operator()(const Geo2& pt)
    {
      Sarry::IPt2 idx = m_cvtr(pt);

      IndexList::iterator it = m_rNeededIdxs.find(idx.y());
      if(it != m_rNeededIdxs.end())
      {
        it->second.first = std::min(it->second.first, idx.x());
        it->second.second = std::max(it->second.second, idx.x());
      }
      else
      {
        IndexList::value_type::second_type xVals(idx.x(), idx.x());
        m_rNeededIdxs.insert(std::make_pair(idx.y(), xVals));
      }
    }

    IndexList& m_rNeededIdxs;
    const Sarry::GeoToIndex& m_cvtr;
  };//end UpdateIdxs
}//end namespace

Sarry::Roi::Roi(const Doi& doi, const std::vector<PerPulseInfo>& infoList,
                const Chirp& chirp, const std::vector<Geo3>& geoPts,
                Meters resolution, const Elevation& elevation,
                RoicToUse roicToUse)
  : m_roic(), m_roiIndices()
{
  //Determine the ROI closure using the DOI.
  std::vector<std::pair<Geo3, Geo3> > roicEnvelope;
  boost::transform(zip(doi.getEnvelope(), infoList),
    std::back_inserter(roicEnvelope),
    unpack(FindRoiClosure(chirp, elevation)));

  //TODO: geoPts should be be stored in a 2D container.
  GeoToIndex cvtr(
      geoPts[0].geo2(), geoPts[1].geo2(), geoPts[3].geo2(), resolution);

  IPt2 maxXPt = cvtr(geoPts[1].geo2());
  IPt2 maxYPt = cvtr(geoPts[3].geo2());
  assert(maxXPt.y() == 0 && "The x-axis coordinate has non-zero y");
  assert(maxYPt.x() == 0 && "The y-axis coordinate has non-zero x");
  int maxX = maxXPt.x();
  int maxY = maxYPt.y();

  IndexList neededIdxs;
  boost::for_each(roicEnvelope, UpdateIdxs(neededIdxs, cvtr)); 

  for(IndexList::iterator it = neededIdxs.begin(), end = neededIdxs.end();
    it != end; ++it)
  {
    int y = it->first;

    int startX = it->second.first;
    int endX = it->second.second;

    if(y >= 0 && y < maxY)
    {
      assert(startX <= 0 && "ROIC not apparently bigger than ROI");
      assert(endX >= maxX && "ROIC not apparently bigger than ROI");

      std::size_t idxBegin = m_roic.size() - startX;
      m_roiIndices.push_back(std::make_pair(idxBegin, idxBegin + maxX));
    }

    if(roicToUse == USE_FULL_ROIC)
    {
      for(int x = startX; x < endX; ++x)
      {
        Geo2 pt = cvtr.reverse(IPt2(x, y));
        m_roic.push_back(Geo3(pt, elevation(pt)));
      }
    }
    else if(y >= -1 && y <= maxY)
    {
      for(int x = startX; x < endX; ++x)
      {
        Geo2 pt = cvtr.reverse(IPt2(x, y));
        m_roic.push_back(Geo3(pt, elevation(pt)));
      }
    }
  }
}

