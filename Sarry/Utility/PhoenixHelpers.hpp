
#include <boost/spirit/home/phoenix.hpp>
#include <cmath>

namespace boost { namespace phoenix {

namespace impl
{
  struct floor
  {
    template<class R>
    struct result { typedef R type; };

    template<class R>
    typename result<R>::type operator()(R& r) const
    {
      using std::floor;
      return floor(r);
    }
  };
}//end impl
function<impl::floor> const floor = impl::floor();

namespace impl
{
  struct abs
  {
    template<class R>
    struct result { typedef R type; };
    template<class R>
    struct result<const std::complex<R> > { typedef R type; };

    template<class R>
    typename result<R>::type operator()(R& r) const
    {
      using std::abs;
      return abs(r);
    }
  };
}//end impl
function<impl::abs> const abs = impl::abs();

namespace impl
{
  struct sqrt
  {
    template<class R>
    struct result { typedef R type; };

    template<class R>
    typename result<R>::type operator()(R& r) const
    {
      using std::sqrt;
      return sqrt(r);
    }
  };
}//end impl
function<impl::sqrt> const sqrt = impl::sqrt();

namespace impl
{
  struct conj
  {
    template<class R>
    struct result { typedef R type; };

    template<class R>
    typename result<R>::type operator()(R& r) const
    {
      using std::conj;
      return conj(r);
    }
  };
}//end impl
function<impl::conj> const conj = impl::conj();

}}//end boost::phoenix

