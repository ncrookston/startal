#ifndef SARRY_UTILITY_ROUND_HPP
#define SARRY_UTILITY_ROUND_HPP

namespace Sarry
{
  template <typename T, typename U> inline T round(U val)
  {
    using std::ceil;
    using std::floor;
    return val - floor(val) > U(.5) ? static_cast<T>(ceil(val))
                                    : static_cast<T>(floor(val));
  }
}

#endif

