#ifndef SARRY_UTILITY_INCSHAREDVALUE_HPP
#define SARRY_UTILITY_INCSHAREDVALUE_HPP

#include <boost/thread/mutex.hpp>

namespace Sarry
{
  class IncSharedValue
  {
  public:
    IncSharedValue() : m_mutex(), m_val(0) {}

    std::size_t increment()
    {
      boost::mutex::scoped_lock l(m_mutex);
      return ++m_val;
    }

    std::size_t get() const
    {
      boost::mutex::scoped_lock l(m_mutex);
      return m_val;
    }

  private:
    mutable boost::mutex m_mutex;
    std::size_t m_val;
  };//end IncrementSharedValue
}//end Sarry

#endif
