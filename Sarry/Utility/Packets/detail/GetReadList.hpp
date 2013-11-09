#ifndef SARRY_PACKETS_DETAIL_GETREADLIST_HPP
#define SARRY_PACKETS_DETAIL_GETREADLIST_HPP

#include <boost/cstdint.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector/vector50.hpp>

namespace Sarry { namespace Packets { namespace detail
{
  using boost::mpl::at;
  using boost::mpl::back_inserter;
  using boost::mpl::insert;
  using boost::mpl::inserter;
  using boost::mpl::pair;
  using boost::mpl::sizeof_;
  using boost::mpl::transform;
  using boost::mpl::vector4;

  typedef boost::mpl::placeholders::_1 mpl1;
  typedef boost::mpl::placeholders::_2 mpl2;

  /** Converts a type T to a pair with its size and type. */
  template <typename T> struct SizeAndType
    : pair<typename sizeof_<T>::type, T> {};

  typedef vector4<boost::uint8_t,  boost::uint16_t,
                  boost::uint32_t, boost::uint64_t> ReadTypes;
  typedef transform
  <
    ReadTypes,
    SizeAndType<mpl1>,
    inserter<boost::mpl::map0<>, insert<mpl1, mpl2> >
  >::type SizesToTypes;

  template <typename T> struct GetReadList : transform
  <T, at<SizesToTypes, sizeof_<mpl1> >,
    back_inserter<boost::mpl::vector0<> >
  >{};

}}}//end Sarry::Packets::detail

#endif

