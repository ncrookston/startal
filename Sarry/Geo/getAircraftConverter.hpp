#ifndef SARRY_GEO_GETAIRCRAFTCONVERTER_HPP
#define SARRY_GEO_GETAIRCRAFTCONVERTER_HPP

#include "Geo/Converter.hpp"
#include "Geo/Euclid.hpp"
#include "Geo/Geo.hpp"
#include "Geo/Orientation.hpp"

namespace Sarry
{
  Converter<Ecef, Nwu> getNwuConverter(Geo3 origin);
  Converter<Nwu, AcftCs> getAircraftConverter(Orientation trajectory);
  Converter<AcftCs, Acs> getAntennaCsConverter(Orientation antenna);

  Converter<Ecef, Acs> getAntennaCsConverter(
        Geo3 origin, Orientation trajectory, Orientation antenna);
}//end Sarry

#endif

