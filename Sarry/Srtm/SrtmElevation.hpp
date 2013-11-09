#ifndef SARRY_GEO_SRTMELEVATION_HPP
#define SARRY_GEO_SRTMELEVATION_HPP

#include "Geo/Elevation.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace GeographicLib
{
  class Geoid;
}

namespace Sarry
{
  class Srtm;

  /** Retrieves elevation from SRTM data (which is relative to the GEOID)
   *  and converts it to be relative to the ellipsoid.
   */
  class SrtmElevation : public Elevation
  {
  public:
    __declspec(dllexport) SrtmElevation(int argc, char** argv);
    /** Declared so it can be defined in a scope where Geoid is defined. */
    __declspec(dllexport) ~SrtmElevation();

    /** Retrieves the elevation (height in meters above the ellipsoid) for the
     *  given lat/lon.
     */
    virtual Meters operator()(const Geo2& pt) const;

  private:
    typedef std::vector<boost::shared_ptr<const Srtm> > SrtmList;
    mutable SrtmList m_srtms;
    boost::scoped_ptr<GeographicLib::Geoid> m_pGeoid;
    std::string m_srtmPath;
  };//end SrtmElevation
}

#endif

