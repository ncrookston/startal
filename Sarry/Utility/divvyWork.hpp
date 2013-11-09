#ifndef SARRY_UTILITY_DIVVYWORK_HPP
#define SARRY_UTILITY_DIVVYWORK_HPP

#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/fusion/include/transform_view.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/range/reference.hpp>
#include <boost/thread.hpp>

namespace Sarry
{
  namespace detail
  {
    template <typename F, typename Tuple>
    struct Worker
    {
      Worker(F f, const Tuple& tuple) : f(f), tuple(tuple) {}

      void operator()() const
      {
        boost::fusion::invoke(f, tuple);
      }

      F f;
      Tuple tuple;
    };//end Worker
    struct GetElement
    {
      GetElement(std::size_t idx) : idx(idx) {}

      template <typename> struct result;
      template <typename F, typename T>
      struct result<F(T&)> : boost::range_reference<T> {};

      template <typename T>
      typename result<GetElement(T&)>::type operator()(T& t) const
      { return t[idx]; }
      
      std::size_t idx;
    };//end GetElement

    template <typename Function, typename Tuple>
    inline
    void divvyWorkImpl(Function f, std::size_t num_threads, std::size_t size,
        Tuple& tuple)
    {
      typedef typename boost::fusion::transform_view<
        Tuple, GetElement> IndivTuple;

      typedef boost::fusion::vector2<std::size_t, std::size_t> IdxTuple;
      typedef boost::fusion::joint_view<IdxTuple, IndivTuple> JoinedTuple;

      std::size_t indexPerThread = size / num_threads;

      boost::thread_group threadList;
      for(std::size_t threadIdx = 0; threadIdx < num_threads - 1; ++threadIdx)
      {
        IdxTuple idxTuple(threadIdx * indexPerThread,
                          (threadIdx + 1) * indexPerThread);
        IndivTuple indivTuple(tuple, GetElement(threadIdx));
        JoinedTuple jTuple(idxTuple, indivTuple);
        threadList.create_thread(Worker<Function, JoinedTuple>(f, jTuple));
      }

      //Execute everything left over in this thread.
      {
        IdxTuple idxTuple((num_threads - 1) * indexPerThread, size);
        IndivTuple indivTuple(tuple, GetElement(num_threads - 1));
        JoinedTuple jTuple(idxTuple, indivTuple);
        boost::fusion::invoke(f, jTuple);
      }

      threadList.join_all();
    }
  }//end detail

  //Requirements on Function -- must be callable with:
  //  f(startIdx, endIdx, args...[idx]).
#define SARRY_UTILITY_PRINT_FUNCTION_ARG(z, n, unused) Arg##n& arg##n
#define SARRY_UTILITY_DIVVY_WORK(z, n, unused) \
  template <typename Function BOOST_PP_COMMA_IF(n) \
    BOOST_PP_ENUM_PARAMS(n, typename Arg) > inline \
  void divvyWork(Function f, std::size_t num_threads, std::size_t size \
        BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM(n, SARRY_UTILITY_PRINT_FUNCTION_ARG, ~)) \
  { \
    detail::divvyWorkImpl(f, num_threads, size, \
       boost::fusion::vector_tie(BOOST_PP_ENUM_PARAMS(n, arg))); \
  }

BOOST_PP_REPEAT(10, SARRY_UTILITY_DIVVY_WORK, ~)
#undef SARRY_UTILITY_DIVVY_WORK
#undef SARRY_UTILITY_PRINT_FUNCTION_ARG

}//end Sarry

#endif
