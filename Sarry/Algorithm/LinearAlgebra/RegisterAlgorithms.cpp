#include "Algorithm/LinearAlgebra/FHd.hpp"
#include "Algorithm/LinearAlgebra/MaximumLikelihood.hpp"
#include "Algorithm/LinearAlgebra/RegionSummary.hpp"
#include "Utility/findInstance.hpp"

namespace ext = boost::extensions;
using namespace Sarry;

BOOST_EXTENSION_TYPE_MAP_FUNCTION
{
  FactoriesMap<Algorithm>::type& algorithm_factories(types.get());
  algorithm_factories["fhd"].set<FHd>();
  algorithm_factories["ml"].set<MaximumLikelihood>();
  algorithm_factories["region_summary"].set<RegionSummary>();
}

