#ifndef SARRY_PACKETS_DETAIL_BUILDSEQUENCE_HPP
#define SARRY_PACKETS_DETAIL_BUILDSEQUENCE_HPP

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/transform_view.hpp>

#define BOOST_TYPEOF_SILENT
#include <boost/typeof/typeof.hpp>

namespace Sarry { namespace Packets { namespace detail {

  using boost::mpl::accumulate;
  using boost::mpl::back;
  using boost::mpl::equal_to;
  using boost::mpl::fold;
  using boost::mpl::plus;
  using boost::mpl::pop_back;
  using boost::mpl::push_back;
  using boost::mpl::sizeof_;
  using boost::mpl::size_t;
  using boost::mpl::transform_view;
  using boost::mpl::vector1;

  typedef boost::mpl::placeholders::_1 mpl1;
  typedef boost::mpl::placeholders::_2 mpl2;

  /** Determines if the MPL sequence T has any padding or not. */
  template <typename T>
  struct HasNoPadding :
    equal_to
    <
      typename accumulate<transform_view<T, sizeof_<mpl1> >,
                          size_t<0>, plus<mpl1, mpl2> >::type,
      typename sizeof_<T>::type
    > {};

  //GetOffsets creates a list of the offsets from the first entry of T to each
  // entry, assuming no padding.
  // Algorithm description:
  //  Start with a vector containing only zero.
  //  Then, for each entry in T (starting with the first), add the size of T
  //  to the last entry of the vector, then append that sum to the vector.
  //  This will create a vector with n + 1 elements, where n is the size of T.
  //  pop_back removes the last element from the vector, (which would otherwise
  //  give the offset to the end of the vector).
  template <typename T>
  struct GetOffsets
    : pop_back
      <
        typename fold
        <
          transform_view<T, sizeof_<mpl1> >, vector1<size_t<0>>,
          push_back<mpl1, plus<back<mpl1>, mpl2> >
        >::type
      >
  {};

  struct ReadFromMem
  {
    ReadFromMem(const unsigned char* const pPacket) : m_pPacket(pPacket) {}

    template <typename T, typename U>
    void operator()(const boost::fusion::vector2<const T&, const U&>& v) const
    {
      T& rT = const_cast<T&>(boost::fusion::at_c<0>(v));
      memcpy(&rT, m_pPacket + boost::fusion::at_c<1>(v), sizeof(T));
    }

    const unsigned char* const m_pPacket;
  };

  /** T contains no padding. */
  template <typename T>
  T buildSequenceImpl(const unsigned char* pPacket, boost::mpl::true_)
  {
    T seq;
    memcpy(&seq, pPacket, sizeof(T));
    return seq;
  }

  /** Case where T is padded. */
  template <typename T>
  T buildSequenceImpl(const unsigned char* pPacket, boost::mpl::false_)
  {
    T seq;
    typename boost::fusion::result_of::as_vector<
      typename GetOffsets<T>::type>::type offsets;
    BOOST_AUTO(valsAndOffsets, boost::fusion::zip(seq, offsets));

    boost::fusion::for_each(valsAndOffsets, ReadFromMem(pPacket));
    return seq;
  }

  template <typename T>
  T buildSequence(const unsigned char* pPacket)
  { return buildSequenceImpl<T>(pPacket, typename HasNoPadding<T>::type()); }

}}}//end Sarry::Packets::detail

#endif

