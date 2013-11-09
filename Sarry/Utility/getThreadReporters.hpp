#ifndef SARRY_UTILITY_GETTHREADREPORTERS_HPP
#define SARRY_UTILITY_GETTHREADREPORTERS_HPP

#include "Utility/IncSharedValue.hpp"
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/thread.hpp>
#include <vector>

namespace Sarry
{
  class Reporter
  {
  public:
    Reporter(boost::shared_ptr<IncSharedValue> pInc,
      std::size_t size,
      const std::string& doneMsg,
      boost::function<void(std::size_t)> cb)
      : pInc(pInc), cb(cb), size(size), doneMsg(doneMsg)
    {}
    Reporter(boost::shared_ptr<IncSharedValue> pInc)
      : pInc(pInc), cb(cb)
    {}

    void operator()() const
    {
      std::size_t val = pInc->increment();
      if(cb)
        cb(val);
    }
    
    void reportUntilDone() const
    {
      if(!cb)
        return;

      std::size_t val = pInc->get();
      while(val < size)
      {
        boost::this_thread::sleep(
          boost::get_system_time() + boost::posix_time::seconds(1));
        val = pInc->get();
        cb(val);
      }
      clearLine(std::clog);
      std::clog << doneMsg << std::endl;
    }

  private:
    boost::shared_ptr<IncSharedValue> pInc;
    boost::function<void(std::size_t)> cb;
    std::size_t size;
    std::string doneMsg;
  };

  inline
  std::vector<Reporter>
    getThreadReporters(std::size_t num_threads, std::size_t size,
        const std::string& progressMsg, const std::string& doneMessage)
  {
    namespace px = boost::phoenix;
    assert(num_threads > 0);
    boost::shared_ptr<IncSharedValue>
      pInc(boost::make_shared<IncSharedValue>());

    std::vector<Reporter> reporters(num_threads - 1, Reporter(pInc));
    reporters.push_back(Reporter(pInc, size, doneMessage,
        (
          px::bind(&clearLine, px::ref(std::clog)),
          px::ref(std::clog) << progressMsg << " done out of " << size << ": "
            << px::arg_names::arg1 << std::flush
        )));

    return reporters; 
  }
}
#endif
