#ifndef SARRY_GEO_CONVERTER_HPP
#define SARRY_GEO_CONVERTER_HPP

#include "Utility/Units.hpp"
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/triangular.hpp>

namespace Sarry
{
  template <typename T, typename U>
  class Converter
  {
  public:
    typedef boost::numeric::ublas::matrix<double> matrix;
    typedef T input_type;
    typedef U output_type;

    Converter(const matrix& tx) : m_tx(tx), m_inversion(false)
    {
      assert(m_tx.size2() == input_type::static_size + 1
        && "Input dimensions mismatch");
      assert(m_tx.size1() == output_type::static_size + 1
        && "Output dimensions mismatch");
    }

    template <typename V>
    Converter(const Converter<input_type, V>& lhs,
              const Converter<V, output_type>& rhs)
      : m_tx(prod(rhs.m_tx, lhs.m_tx)), m_inversion(false)
    {}

    output_type operator()(const input_type& in) const
    { return m_inversion ? backward(in) : forward(in); }

    Converter<output_type, input_type> invert() const
    {
      matrix lu = m_tx;
      lu_factorize(lu);
      Converter<output_type, input_type> cvtr(lu);
      cvtr.m_inversion = true;

      return cvtr;
    }

  private:
    template <typename V, typename W> friend class Converter;

    output_type backward(const input_type& in) const
    {
      assert(m_inversion && "Calling backward incorrectly");
      matrix iv(4,1);
      for(std::size_t i = 0; i < input_type::static_size; ++i)
        iv(i,0) = in.data[i].value();
      iv(input_type::static_size,0) = 1;

      lu_substitute(m_tx, iv);

      output_type out;
      for(std::size_t i = 0; i < output_type::static_size; ++i)
        out.data[i] = iv(i,0) / iv(output_type::static_size,0) * si::meters;
      return out;
    }

    output_type forward(const input_type& in) const
    {
      assert(!m_inversion && "Calling forward incorrectly");
      matrix iv(4,1);
      for(std::size_t i = 0; i < input_type::static_size; ++i)
        iv(i,0) = in.data[i].value();
      iv(input_type::static_size,0) = 1;

      BOOST_AUTO(ov, prod(m_tx, iv));

      output_type out;
      for(std::size_t i = 0; i < output_type::static_size; ++i)
        out.data[i] = ov(i,0) / ov(output_type::static_size,0) * si::meters;
      return out;
    }


    matrix m_tx;
    bool m_inversion;
  };
}//end Sarry

#endif

