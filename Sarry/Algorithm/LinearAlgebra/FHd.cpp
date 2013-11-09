
#include "FHd.hpp"
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/MatchedMatrix.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Antenna/Antenna.hpp"
#include "Antenna/PerPulseInfo.hpp"
#include "Geo/Elevation.hpp"
#include "Utility/clearLine.hpp"
#include "Utility/IncSharedValue.hpp"

#include <boost/range/distance.hpp>
#include <boost/range/size.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <fstream>
#include <iostream>

using namespace Sarry;
namespace px = boost::phoenix;
using px::arg_names::arg1;

namespace
{
  template <typename Range1, typename Range2, typename Reporter>
  struct ComputeCBP
  {
    ComputeCBP(const MatchedMatrix& f_, const Doi& doi_, const Range1& gIdcs_,
               Range2& gVals_, std::size_t startIdx_,
               std::size_t endIdx_, Reporter report_)
      : f(f_), doi(doi_), gIdcs(gIdcs_), gVals(gVals_), startIdx(startIdx_),
        endIdx(endIdx_), report(report_)
    {}

    void operator()()
    {
      try
      {
        std::size_t az = 0;
        for(Roi::IdxPairList::const_iterator it = boost::begin(gIdcs)+startIdx,
            end = boost::begin(gIdcs) + endIdx; it != end; ++it, ++az)
        {
          std::size_t elVals = it->second - it->first;
          std::size_t el = 0;
          for(std::size_t geoIdx = it->first; geoIdx != it->second;
            ++geoIdx, ++el)
          {
            for(std::size_t dataIdx = 0; dataIdx != doi.getSamples().size();
              ++dataIdx)
            {
              //if(boost::distance(gIdcs) <= std::distance(boost::begin(gIdcs), it))
              //  throw std::runtime_error("Out of range az value.");
              //if(static_cast<std::size_t>(boost::size(gVals)) <= (startIdx + az) * elVals + el)
              //  throw std::runtime_error("Out of range g value.");
              //if(f.getRoi().getRoic().size() <= geoIdx)
              //  throw std::runtime_error("Out of range geoIdx value.");
              gVals[(startIdx + az) * elVals + el]
                += conj(f(dataIdx, geoIdx)) * doi.getSamples()[dataIdx].data;
            }

            report();
          }
        }
      }
      catch(std::exception& rEx)
      {
        std::cerr << "Exception in processing: " << rEx.what() << std::endl;
      }
    }

    MatchedMatrix f;
    const Doi& doi;
    const Range1& gIdcs;
    Range2& gVals;
    std::size_t startIdx;
    std::size_t endIdx;
    Reporter report;

  };//end ComputeCBP

  template <typename Range1, typename Range2, typename Reporter>
  ComputeCBP<Range1, Range2, Reporter> computeCBP(
    const MatchedMatrix& f, const Doi& doi, const Range1& gIdcs,
    Range2& gVals, std::size_t startIdx, std::size_t endIdx,
    Reporter report)
  {
    return ComputeCBP<Range1, Range2, Reporter>(f, doi, gIdcs, gVals,
      startIdx, endIdx, report);
  }
}//end namespace

FHd::FHd(int argc, char** argv, const Antenna& antenna,
         const Elevation& elevation)
  : LinearAlgebra(argc, argv, antenna, elevation)
{}

FHd::AlgResult FHd::applyAlgorithm(const std::vector<PerPulseInfo>& pulseInfo,
    const Doi& doi, const Roi& roic) const
{
  const Roi::IdxPairList& gIdcs = roic.getRoiIndexEnvelope();
  std::ptrdiff_t azVals = boost::distance(gIdcs);
  if(azVals == 0)
    throw std::invalid_argument("No rows shown in ROI index envelope");

  std::ptrdiff_t elVals = gIdcs.front().second - gIdcs.front().first;

  std::vector<std::complex<data_type> > gVals(azVals * elVals,
    std::complex<data_type>(0,0));

  std::clog << "Number of output pixels: " << gVals.size() << std::endl;
  std::clog << "Size of d vector: " << doi.getSamples().size() << std::endl;

  std::clog << "F matrix being created." << std::endl;
  MatchedMatrix f(pulseInfo, m_chirp, doi, roic);

  std::size_t num_threads = boost::thread::hardware_concurrency();
  if(num_threads == 0)
    num_threads = 1;

  IncSharedValue incrementer;

  std::size_t indexPerThread = azVals / num_threads;

  boost::thread_group threadList;
  for(std::size_t threadIdx = 0; threadIdx < num_threads - 1; ++threadIdx)
  {
    std::size_t startIdx = threadIdx * indexPerThread;
    std::size_t endIdx = (threadIdx + 1) * indexPerThread;
    threadList.create_thread(
      computeCBP(f, doi, gIdcs, gVals, startIdx, endIdx,
        px::bind(&IncSharedValue::increment, &incrementer)));
  }
  //Execute everything left over in this thread.
  {
    std::size_t startIdx = (num_threads - 1) * indexPerThread;
    computeCBP(f, doi, gIdcs, gVals, startIdx, gIdcs.size(),
      (px::bind(&IncSharedValue::increment, &incrementer),
       px::bind(&clearLine, boost::ref(std::clog)),
       px::ref(std::clog) << "Completed output pixel: "
          << px::bind(&IncSharedValue::get, &incrementer) << std::flush))();
  }

  threadList.join_all();

  clearLine(std::clog);
  std::clog << "Completed output pixels." << std::endl;

  return std::make_pair(gVals, static_cast<std::size_t>(elVals));
}

