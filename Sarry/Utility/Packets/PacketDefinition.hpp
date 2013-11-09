#ifndef SARRY_PACKETS_PACKETDEFINITION_HPP
#define SARRY_PACKETS_PACKETDEFINITION_HPP

#include "Utility/Endian.hpp"

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/vector/vector50.hpp>

namespace Sarry { namespace Packets { namespace detail {
template <typename T>
struct GetHandlingList;
}}}//end Sarry::Utility::detail

/** Detail definitions, do not use. */

/** Adds the last tuple element to a list */
#define SARRY_DETAIL_SELECT_HANDLING(R, _, I, tuple)                          \
  BOOST_PP_COMMA_IF(I) BOOST_PP_TUPLE_ELEM(3,2,tuple)

/** Copies a 3-tuple exactly as-is to be reused by another PP command */
#define SARRY_DETAIL_FILLER_0(X, Y, Z)                                        \
    ((X, Y, Z)) SARRY_DETAIL_FILLER_1
#define SARRY_DETAIL_FILLER_1(X, Y, Z)                                        \
    ((X, Y, Z)) SARRY_DETAIL_FILLER_0
#define SARRY_DETAIL_FILLER_0_END
#define SARRY_DETAIL_FILLER_1_END

/** Converts a list of 3-tuples to a list of 2-tuples */
#define SARRY_DETAIL_REMOVE_LAST_0(X, Y, Z)                                   \
    (X, Y) SARRY_DETAIL_REMOVE_LAST_1
#define SARRY_DETAIL_REMOVE_LAST_1(X, Y, Z)                                   \
    (X, Y) SARRY_DETAIL_REMOVE_LAST_0
#define SARRY_DETAIL_REMOVE_LAST_0_END
#define SARRY_DETAIL_REMOVE_LAST_1_END

/** Given a sequence of tuples, creates a comma-separated list of the last
 *  tuple element in the sequence. */
#define SARRY_DETAIL_LIST_HANDLING(ATTR_SEQ)                                  \
  BOOST_PP_SEQ_FOR_EACH_I(SARRY_DETAIL_SELECT_HANDLING, ~, ATTR_SEQ)

/** End detail definitions. */

/** Use this to define a new packet. */
#define SARRY_PACKET(NAME, ATTRIBUTES)                                        \
  BOOST_FUSION_DEFINE_STRUCT((Sarry)(Packets), NAME,                          \
      BOOST_PP_CAT(SARRY_DETAIL_REMOVE_LAST_0 ATTRIBUTES,_END));              \
  namespace Sarry { namespace Packets { namespace detail {                    \
    template <> struct GetHandlingList<Sarry::Packets::NAME>                  \
    { typedef boost::mpl::BOOST_PP_CAT(vector,                                \
      BOOST_PP_SEQ_SIZE(BOOST_PP_CAT(SARRY_DETAIL_FILLER_0 ATTRIBUTES,_END)))<\
        SARRY_DETAIL_LIST_HANDLING(                                           \
            BOOST_PP_CAT(SARRY_DETAIL_FILLER_0 ATTRIBUTES,_END))              \
      > type;                                                                 \
    };                                                                        \
  }}}
#endif

