#ifndef SARRY_GEO_VEC_HPP
#define SARRY_GEO_VEC_HPP

#include <boost/array.hpp>
#include <boost/static_assert.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#define BOOST_TYPEOF_SILENT
#include <boost/typeof/typeof.hpp>
#include <cstddef>

namespace Sarry
{
  /** Very arbitrary point type -- may be tagged for accessibility (see
   *  SARRY_VEC_ACCESSOR(), which operators should be defined for it, etc.
   */
  template <typename TAG, typename T, std::size_t N>
  struct Vec
  {
    typedef TAG type;
    typedef T value_type;
    static const std::size_t static_size = N;

    /** Initialize to all zeros */
    Vec() : data()
    { std::fill(data.begin(), data.end(), T()); }

    /** TODO: Add a converting copy constructor. */
    Vec(const Vec<TAG, T, N>& rhs) : data(rhs.data)
    {}

    explicit Vec(T t1) : data()
    {
      BOOST_STATIC_ASSERT(1 == N);
      data[0] = t1;
    }

    Vec(T t1, T t2) : data()
    {
      BOOST_STATIC_ASSERT(2 == N);
      data[0] = t1;
      data[1] = t2;
    }

    Vec(T t1, T t2, T t3) : data()
    {
      BOOST_STATIC_ASSERT(3 == N);
      data[0] = t1;
      data[1] = t2;
      data[2] = t3;
    }

    //Declare some accessor functions
#define SARRY_VEC_ACCESSOR(name, idx)           \
  T& name()                                     \
  {                                             \
    BOOST_STATIC_ASSERT(idx < static_size);     \
    return data[idx];                           \
  }                                             \
  const T& name() const                         \
  {                                             \
    BOOST_STATIC_ASSERT(idx < static_size);     \
    return data[idx];                           \
  }

    SARRY_VEC_ACCESSOR(x,0);
    SARRY_VEC_ACCESSOR(y,1);
    SARRY_VEC_ACCESSOR(z,2);

#undef SARRY_VEC_ACCESSOR

  private:
    template <typename T, typename U> friend class Converter;

    friend Vec<TAG,T,N> operator+(
        const Vec<TAG,T,N>& lhs, const Vec<TAG,T,N>& rhs)
    {
      Vec<TAG,T,N> r;
      for(std::size_t i = 0; i < N; ++i)
        r.data[i] = lhs.data[i] + rhs.data[i];
      return r;
    }

    friend Vec<TAG,T,N> operator-(
        const Vec<TAG,T,N>& lhs, const Vec<TAG,T,N>& rhs)
    {
      Vec<TAG,T,N> r;
      for(std::size_t i = 0; i < N; ++i)
        r.data[i] = lhs.data[i] - rhs.data[i];
      return r;
    }

    friend std::ostream& operator<<(std::ostream& out, const Vec<TAG,T,N>& vec)
    {
      out << '(';
      for(std::size_t i = 0; i < N; ++i)
      {
        out << vec.data[i];
        if(i + 1 != N)
          out << ", ";
      }
      out << ')';
      return out;
    }

    friend BOOST_TYPEOF(T() * T()) inner_product(
      const Vec<TAG, T, N>& lhs, const Vec<TAG, T, N>& rhs)
    {
      typedef BOOST_TYPEOF(T() * T()) type;
      type sum = type();
      for(std::size_t i = 0; i < N; ++i)
        sum += lhs.data[i] * rhs.data[i];
      return sum;
    }

    friend T distance(const Vec<TAG, T, N>& val)
    {
      using std::sqrt;
      return sqrt(inner_product(val, val));
    }

    /** The data stored by this vector. */
    boost::array<T, N> data;
  };//end Vec

}//end Sarry

#endif

