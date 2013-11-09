
#include "Utility/Endian.hpp"

#include <limits>

#include <boost/type_traits/make_unsigned.hpp>
#include <boost/test/unit_test.hpp>

using namespace Sarry;

template <typename T> struct GetSameSizeInteger;
template <> struct GetSameSizeInteger<float> { typedef boost::uint32_t type; };
template <> struct GetSameSizeInteger<double> { typedef boost::uint64_t type; };

template <bool isSigned>
struct TestSwap
{
  template <typename T>
  static void apply(T& rVal)
  {
    typename boost::make_unsigned<T>::type valU;
    memcpy(&valU, &rVal, sizeof(T));
    Utility::detail::SwapIfNeeded<
      typename boost::make_unsigned<T>::type, Utility::big_endian>::apply(valU);
    memcpy(&rVal, &valU, sizeof(T));
  }
};
template <>
struct TestSwap<false>
{
  template <typename T>
  static void apply(T& rVal)
  {
    Utility::detail::SwapIfNeeded<T, Utility::big_endian>::apply(rVal);
  }
};

template <typename T>
void testSwap(T orig, T swapped)
{
  BOOST_STATIC_ASSERT(boost::is_integral<T>::value);

  TestSwap<boost::is_signed<T>::value>::apply(orig);
  BOOST_CHECK_EQUAL(orig, swapped);
}

template <bool IS_INTEGER>
struct TestRoundTrip
{
  template <typename T>
  static void apply(T val)
  {
    T oldVal = val;
    TestSwap<boost::is_signed<T>::value>::apply(val);
    TestSwap<boost::is_signed<T>::value>::apply(val);

    BOOST_CHECK_EQUAL(oldVal, val);
  }
};

template <>
struct TestRoundTrip<false>
{
  template <typename T>
  static void apply(T val)
  {
    typedef typename GetSameSizeInteger<T>::type Int;
    BOOST_STATIC_ASSERT(sizeof(Int) == sizeof(T));
    Int intVal;
    memcpy(&intVal, &val, sizeof(T));

    TestSwap<boost::is_signed<Int>::value>::apply(intVal);
    TestSwap<boost::is_signed<Int>::value>::apply(intVal);
    T newVal;
    memcpy(&newVal, &intVal, sizeof(T));

    BOOST_CHECK_EQUAL(newVal, val);
  }
};

template <typename T>
void testRoundTrip(T val)
{
  TestRoundTrip<boost::is_integral<T>::value>::apply(val);
}

BOOST_AUTO_TEST_CASE( Sarry_EndianTest )
{
  testSwap<boost::uint16_t>(512, 2);
  testSwap<boost::int16_t>(-512, 254);
  testSwap<boost::uint64_t>(3819052484010180608ULL, 53ULL);
  testSwap<boost::int64_t>(3819052484010180608LL, 53LL);
  testSwap<boost::int64_t>(-255, 0x01ffffffffffffffLL);
  testSwap<boost::int64_t>(std::numeric_limits<boost::int64_t>::min(),
                           128LL);

  const char str[] = "\x80\x00\x00\x00\x00\x00\x00\x00";
  boost::int64_t val;
  BOOST_STATIC_ASSERT(sizeof(str) == sizeof(boost::int64_t) + 1);
  memcpy(&val, str, sizeof(str) - 1);
  testSwap<boost::int64_t>(val, std::numeric_limits<boost::int64_t>::min());

  testRoundTrip(1.23456);
  testRoundTrip(1.23456f);
  testRoundTrip<boost::uint16_t>(512);
  testRoundTrip<boost::int16_t>(-512);
}

