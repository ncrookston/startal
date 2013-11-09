
#include "Algorithm/LinearAlgebra/Doi.hpp"
#include "Algorithm/LinearAlgebra/MatchedMatrix.hpp"
#include "Algorithm/LinearAlgebra/MaximumLikelihood.hpp"
#include "Algorithm/LinearAlgebra/Roi.hpp"
#include "Antenna/Antenna.hpp"
#include "Antenna/getPulsesOfInterest.hpp"
#include "Utility/clearLine.hpp"
#include "Utility/divvyWork.hpp"
#include "Utility/getThreadReporters.hpp"
#include "Utility/writeMagImage.hpp"

#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/distance.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <iostream>

namespace ublas = boost::numeric::ublas;
using boost::adaptors::transformed;
using boost::units::quantity_cast;
namespace po = boost::program_options;
namespace px = boost::phoenix;
using px::arg_names::arg1;
using px::arg_names::arg2;
using px::arg_names::arg3;
using namespace Sarry;

namespace
{
  typedef ublas::matrix<std::complex<data_type> > ComplexMatrix;
  typedef ublas::vector<ComplexMatrix::value_type> ComplexVector;

  void initFh(ComplexMatrix& rFh, MatchedMatrix f,
    std::size_t beginRoic, std::size_t endRoic, std::size_t doiSize,
    Reporter reporter)
  {
    for(std::size_t roic = beginRoic; roic < endRoic; ++roic)
    {
      for(std::size_t doi = 0; doi < doiSize; ++doi)
      {
        rFh(roic, doi) = conj(f(doi, roic));
      }
      reporter();
    }
    reporter.reportUntilDone();
  }

  void multFhF(ComplexMatrix& rFhF, const ComplexMatrix& Fh,
               const ComplexMatrix& F, std::size_t roicBegin,
               std::size_t roicEnd, Reporter reporter)
  {
    for(std::size_t r = roicBegin; r < roicEnd; ++r)
    {
      for(std::size_t c = 0; c < rFhF.size2(); ++c)
      {
        rFhF(r, c) = inner_prod(row(Fh, r), column(F, c));
        reporter();
      }
    }
    reporter.reportUntilDone();
//    subrange(rFhF, roicBegin, roicEnd, 0, rFhF.size2()) =
//      prod(subrange(Fh, roicBegin, roicEnd, 0, Fh.size2()), F);
  }

  void multFhd(ComplexVector& rFhd, const ComplexMatrix& Fh,
               const ComplexVector& d, std::size_t roicBegin,
               std::size_t roicEnd, Reporter reporter)
  {
    for(std::size_t r = roicBegin; r < roicEnd; ++r)
    {
      rFhd(r) = inner_prod(row(Fh, r), d);
      reporter();
    }
    reporter.reportUntilDone();
  }

}//end namespace

Sarry::MaximumLikelihood::MaximumLikelihood(int argc, char** argv,
    const Antenna& antenna, const Elevation& elevation)
  : LinearAlgebra(argc, argv, antenna, elevation)
{}

//virtual method
Sarry::MaximumLikelihood::AlgResult Sarry::MaximumLikelihood::applyAlgorithm(
    const std::vector<PerPulseInfo>& pulseInfo,
    const Doi& doi, const Roi& roic) const
{
  std::size_t doiSize = doi.getSamples().size();
  std::size_t roicSize = roic.getRoic().size();

  std::clog << "Size of g vector: " << roicSize << std::endl;
  std::clog << "Size of d vector: " << doiSize << std::endl;

  std::clog << "Creating F. . ." << std::endl;
  MatchedMatrix f(pulseInfo, m_chirp, doi, roic);
  
  ublas::matrix<std::complex<data_type> > FhF(roicSize, roicSize);
  ublas::vector<std::complex<data_type> > Fhd(roicSize);

  std::size_t num_threads =
    std::max<std::size_t>(boost::thread::hardware_concurrency(), 1);

  //Because F and Fh are large, reduce their scope.
  {
    ublas::matrix<std::complex<data_type> > Fh(roicSize, doiSize);
    
     BOOST_AUTO(reporters, getThreadReporters(num_threads, roicSize,
          "Rows", "All rows completed"));
    divvyWork(boost::bind(&initFh, boost::ref(Fh), f, _1, _2, doiSize, _3),
        num_threads, roicSize, reporters);

    //Accessing herm(F) is cache inefficient -- 
    ublas::vector<std::complex<data_type> > d(doiSize);
    boost::copy(doi.getSamples() | transformed(px::bind(&KN::data, arg1)),
        d.begin());

    ublas::matrix<std::complex<data_type> > F = herm(F);

    std::clog << "Creating F^H * d. . ." << std::endl;
    reporters = getThreadReporters(num_threads, roicSize,
          "Rows", "All rows completed");
    divvyWork(boost::bind(&multFhd, boost::ref(Fhd), boost::cref(Fh),
      boost::cref(d), _1, _2, _3), num_threads, roicSize, reporters);

    std::clog << "Creating F^H * F. . ." << std::endl;
    reporters = getThreadReporters(num_threads, roicSize * roicSize,
      "Elements", "All rows completed");
    divvyWork(boost::bind(&multFhF, boost::ref(FhF), boost::cref(Fh),
              boost::cref(F), _1, _2, _3), num_threads, roicSize,
              reporters);

  }

  //Write some debug output in case the matrices need conditioning, etc.
  {
    std::ofstream outInfo("about.log"), outFhd("Fhd.bin", std::ios::binary),
      outFhF("FhF.bin", std::ios::binary);
    if(!outInfo.is_open() || !outFhd.is_open() || ! outFhF.is_open())
      throw std::runtime_error("Cannot open files");

    outInfo << "Size of Fhd.bin vector:\n"
      << Fhd.size() << std::endl
      << "Sizes of FhF.bin matrix:\n"
      << FhF.size1() << " rows x " << FhF.size2() << " columns.";

    outFhd.write(reinterpret_cast<const char*>(&Fhd.data()[0]),
      sizeof(ComplexVector::value_type) * Fhd.data().size());

    outFhF.write(reinterpret_cast<const char*>(&FhF.data()[0]),
      sizeof(ComplexMatrix::value_type) * Fhd.data().size());
  }

  //If only I knew how to thread this. . .
  std::clog << "Performing LU factorization of F^H * F. . ." << std::endl;
  ublas::lu_factorize(FhF);

  std::clog << "Performing LU back substitution of (F^H * F)^(-1). . .\n";
  //Cast to const& required to avoid ambiguity.
  ublas::lu_substitute(
    static_cast<const ublas::matrix<std::complex<data_type> >&>(FhF), Fhd);

  const Roi::IdxPairList& gIdcs = roic.getRoiIndexEnvelope();

  std::ptrdiff_t azVals = boost::distance(gIdcs);
  if(azVals == 0)
    throw std::invalid_argument("No rows shown in ROI index envelope");

  std::ptrdiff_t elVals = gIdcs.front().second - gIdcs.front().first;

  std::vector<std::complex<data_type> > gVals(azVals * elVals,
    std::complex<data_type>(0,0));

  std::clog << "Creating ROI from ROIC. . ." << std::endl;
  std::size_t az = 0;
  for(Roi::IdxPairList::const_iterator it = boost::begin(gIdcs),
    end = boost::end(gIdcs); it != end; ++it, ++az)
  {
    std::size_t el = 0;
    for(std::size_t geoIdx = it->first; geoIdx != it->second;
      ++geoIdx, ++el)
    {
      gVals[az * elVals + el] += Fhd(geoIdx);
    }
  }

  return std::make_pair(gVals, static_cast<std::size_t>(elVals));
}

