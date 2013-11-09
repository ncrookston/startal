#ifndef SARRY_PACKETS_GET_HPP
#define SARRY_PACKETS_GET_HPP

#include "Utility/Endian.hpp"
#include "Utility/Packets/detail/buildSequence.hpp"
#include "Utility/Packets/detail/GetReadList.hpp"
#include "Utility/Packets/PacketDefinition.hpp"

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/support/is_sequence.hpp>

#include <boost/mpl/size.hpp>
#include <boost/mpl/assert.hpp>

namespace Sarry { namespace Packets {

namespace detail
{
  struct SpecialHandling
  {
    template <typename T, typename U>
    void operator()(const boost::fusion::vector2<const T&, const U&>& v) const
    { handle(const_cast<U&>(boost::fusion::at_c<1>(v)), T()); }
  };
}//end detail

template <typename T>
T get(const unsigned char* pPacket)
{
  BOOST_MPL_ASSERT((boost::fusion::traits::is_sequence<T>));

  //Get fusion sequences of types that are length-compatible with T but are
  // unsigned and integral.
  typedef typename boost::fusion::result_of::as_vector<
    typename detail::GetReadList<T>::type >::type ToRead;

  //Also retrieve the list indicating the endianness of each type T.
  typedef typename boost::fusion::result_of::as_vector<
    typename detail::GetHandlingList<T>::type>::type HandlingTypes;

  BOOST_AUTO(tr, detail::buildSequence<ToRead>(pPacket));

  using boost::mpl::size;
  //Make sure the length of the sequences match.
  BOOST_MPL_ASSERT_RELATION(size<ToRead>::value, ==, size<T>::value);
  //Make sure number of bytes matches -- this may include padding, but the
  // padding should be equal for both.
  BOOST_MPL_ASSERT_RELATION(sizeof(tr), ==, sizeof(T));

  HandlingTypes ht;
  for_each(boost::fusion::zip(ht, tr), detail::SpecialHandling());

  //No need to worry about pad pixels here, that was taken care of by
  // buildSequence.
  T t;
  memcpy(&t, &tr, sizeof(tr));
  return t;
}

}}//end Sarry::Packets

#endif

