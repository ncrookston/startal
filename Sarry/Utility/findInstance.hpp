#ifndef SARRY_UTILITY_FINDINSTANCE_HPP
#define SARRY_UTILITY_FINDINSTANCE_HPP

#include <boost/extension/extension.hpp>
#include <boost/extension/factory.hpp>
#include <boost/extension/type_map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <map>
#include <string>

namespace Sarry
{
#define PRINT_ARG(z, n, seq) typename boost::mpl::at_c<seq, n>::type
#define PRINT_FUNCTION_ARG(z, n, unused) Arg##n arg##n

  namespace detail
  {
    template <typename Factory, std::size_t>
    struct FactoryType;

#define CREATE_FACTORY_TYPE(z, n, unused) \
    template <typename Factory> \
    struct FactoryType<Factory, n> \
    { \
      typedef boost::extensions::factory<Factory \
        BOOST_PP_COMMA_IF(n)  \
        BOOST_PP_ENUM(n, PRINT_ARG, typename Factory::constructor_arguments)> type; \
    };
BOOST_PP_REPEAT(10, CREATE_FACTORY_TYPE, ~)
#undef CREATE_FACTORY_TYPE

  }//end detail

  template <typename Factory>
  struct FactoriesMap
  {
    typedef typename boost::mpl::size<
      typename Factory::constructor_arguments>::type Size;
    typedef std::map<std::string, typename detail::FactoryType<Factory,
      Size::value
    >::type> type;

  };//end FactoriesMap

#define FIND_INSTANCE(z, n, unused) \
  template <typename T BOOST_PP_COMMA_IF(n) \
  BOOST_PP_ENUM_PARAMS(n, typename Arg)> inline \
  T* findInstance(boost::extensions::type_map& rTypes, \
        const std::string& type \
        BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM(n, PRINT_FUNCTION_ARG, ~)) \
  { \
    typedef typename FactoriesMap<T>::type MapType; \
    MapType& factories(rTypes.get()); \
 \
    T* pInstance = NULL; \
    typename MapType::iterator it = factories.find(type); \
    if(it != factories.end()) \
      pInstance = it->second.create(BOOST_PP_ENUM_PARAMS(n, arg)); \
 \
    return pInstance; \
  }

BOOST_PP_REPEAT(10, FIND_INSTANCE, ~)

#undef FIND_INSTANCE
#undef PRINT_FUNCTION_ARG
#undef PRINT_ARG
}//end Sarry

#endif
