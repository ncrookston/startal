#ifndef SARRY_UTILITY_CLEARLINE_HPP
#define SARRY_UTILITY_CLEARLINE_HPP

namespace Sarry
{
  inline void clearLine(std::ostream& rOut)
  {
    rOut << "\xd                                                 \xd"
         << std::flush;
  }
}//end Sarry
#endif
