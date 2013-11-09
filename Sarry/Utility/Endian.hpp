#ifndef SARRY_UTILITY_ENDIAN_HPP
#define SARRY_UTILITY_ENDIAN_HPP

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_signed.hpp>

namespace Sarry { namespace Utility {

//Tags indicating if something is big or little endian.
struct big_endian {};
struct little_endian {};

#ifdef BOOST_LITTLE_ENDIAN
typedef little_endian HostEndian;
#else
typedef big_endian HostEndian;
#endif

//Taken from: http://www.codeguru.com/forum/showthread.php?t=292902
inline void endian_swap(boost::uint16_t& x)
{ x = (x>>8) | (x<<8); }

inline void endian_swap(boost::uint32_t& x)
{ x = (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24); }

inline void endian_swap(boost::uint64_t& x)
{
  x = (x>>56) |
      ((x>>40) & 0x000000000000FF00ULL) |
      ((x>>24) & 0x0000000000FF0000ULL) |
      ((x>>8)  & 0x00000000FF000000ULL) |
      ((x<<8)  & 0x000000FF00000000ULL) |
      ((x<<24) & 0x0000FF0000000000ULL) |
      ((x<<40) & 0x00FF000000000000ULL) |
      (x<<56);
}
//end others' code

namespace detail
{
  template <typename T, bool DO_SWAP>
  struct SINImpl { void operator()(T&) {} };

  //If native endianness is little, create some overloads.
  template <> struct SINImpl<boost::uint16_t, true>
  { void operator()(boost::uint16_t& rT) { endian_swap(rT); } };
  template <> struct SINImpl<boost::uint32_t, true>
  { void operator()(boost::uint32_t& rT) { endian_swap(rT); } };
  template <> struct SINImpl<boost::uint64_t, true>
  { void operator()(boost::uint64_t& rT) { endian_swap(rT); } };

  template <typename T, typename END>
  struct SwapIfNeeded
  {
    static void apply(T& t)
    {
      //We only operate on unsigned integeral values.
      BOOST_STATIC_ASSERT(!boost::is_signed<T>::value &&
                          boost::is_integral<T>::value);

      //If the host is little and the data is big, swap.  Likewise if
      // host is big and data is little.
      detail::SINImpl<T, !boost::is_same<HostEndian, END>::value>()(t);
    }
  };

}

/** handle is found through ADL.  The correct overload is found via tag
 *  dispatch. */
template <typename T> void handle(T& t, little_endian)
{ detail::SwapIfNeeded<T, little_endian>::apply(t); }

template <typename T> void handle(T& t, big_endian)
{ detail::SwapIfNeeded<T, big_endian>::apply(t); }

}}//end Sarry::Utility

#endif

