#ifndef SARRY_GEO_WRITEPOINTLIST_HPP
#define SARRY_GEO_WRITEPOINTLIST_HPP

#include "Geo/Geo.hpp"
#include "Geo/toKmlString.hpp"

#include <boost/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <fstream>
#include <string>

namespace Sarry
{
  namespace detail
  {
    void printPlacemark(std::string& rOut, const Geo3& pt,
        const std::string& style)
    {
      rOut += "<Placemark>\n";
      rOut += "  <name>pt</name>\n";
      rOut += "  <styleUrl>#" + style + "</styleUrl>\n";
      rOut += "  <Point>\n";
      rOut += "    <coordinates>" + toKmlString(pt) + "</coordinates>\n";
      rOut += "  </Point>\n";
      rOut += "</Placemark>\n";
    }
  }//end detail

  template <typename RANGE>
  std::string writePointList(const std::string& name, const RANGE& range,
      const std::string& style)
  {
    std::string out = "  <Folder>\n";
    out += "    <name>" + name + "</name>\n";

    boost::for_each(range,
      boost::bind(detail::printPlacemark, boost::ref(out), _1, style));

    out += "   </Folder>\n";

    return out;
  }

}//end Sarry

#endif

